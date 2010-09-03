/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile$

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMSessionCore.h"

#include "vtkClientServerID.h"
#include "vtkClientServerInterpreter.h"
#include "vtkClientServerInterpreterInitializer.h"
#include "vtkClientServerStream.h"
#include "vtkInstantiator.h"
#include "vtkMultiProcessController.h"
#include "vtkMultiProcessStream.h"
#include "vtkObjectFactory.h"
#include "vtkPMProxy.h"
#include "vtkProcessModule2.h"
#include "vtkPVInformation.h"
#include "vtkSmartPointer.h"
#include "vtkSMProxyDefinitionManager.h"

#include <vtkstd/string>
#include "assert.h"

namespace
{
  void RMICallback(void *localArg, void *remoteArg,
    int vtkNotUsed(remoteArgLength), int vtkNotUsed(remoteProcessId))
    {
    vtkSMSessionCore* sessioncore = reinterpret_cast<vtkSMSessionCore*>(localArg);
    unsigned char type = *(reinterpret_cast<unsigned char*>(remoteArg));
    switch (type)
      {
    case vtkSMSessionCore::PUSH_STATE:
      sessioncore->PushStateSatelliteCallback();
      break;

    case vtkSMSessionCore::GATHER_INFORMATION:
      sessioncore->GatherInformationStatelliteCallback();
      break;
      }
    }
};
//****************************************************************************/
//                        Internal Class
//****************************************************************************/
class vtkSMSessionCore::vtkInternals
{
public:

  //---------------------------------------------------------------------------
  void Delete(vtkTypeUInt32 globalUniqueId)
    {
    PMObjectMapType::iterator iter = this->PMObjectMap.find(globalUniqueId);
    if (iter != this->PMObjectMap.end())
      {
      iter->second.GetPointer()->Finalize();
      this->PMObjectMap.erase(iter);
      }
    }

  //---------------------------------------------------------------------------
  vtkPMObject* GetPMObject(vtkTypeUInt32 globalUniqueId)
    {
    PMObjectMapType::iterator iter = this->PMObjectMap.find(globalUniqueId);
    if (iter != this->PMObjectMap.end())
      {
      return iter->second;
      }

    return NULL; // Did not find it
    }
  //---------------------------------------------------------------------------
  typedef vtkstd::map<vtkTypeUInt32, vtkSmartPointer<vtkPMObject> >
    PMObjectMapType;
  PMObjectMapType PMObjectMap;
};

//****************************************************************************/
vtkStandardNewMacro(vtkSMSessionCore);
//----------------------------------------------------------------------------
vtkSMSessionCore::vtkSMSessionCore()
{
  this->Interpreter =
    vtkClientServerInterpreterInitializer::GetInitializer()->NewInterpreter();
  this->Internals = new vtkInternals();

  this->ProxyDefinitionManager = vtkSMProxyDefinitionManager::New();

  this->ParallelController = vtkMultiProcessController::GetGlobalController();
  if (this->ParallelController &&
    this->ParallelController->GetLocalProcessId() > 0)
    {
    this->ParallelController->AddRMI(&RMICallback, this,
      ROOT_SATELLITE_RMI_TAG);
    }
}

//----------------------------------------------------------------------------
vtkSMSessionCore::~vtkSMSessionCore()
{
  this->Interpreter->Delete();
  this->Interpreter = 0;
  if (this->ParallelController &&
    this->ParallelController->GetLocalProcessId() == 0)
    {
    this->ParallelController->TriggerBreakRMIs();
    }
  delete this->Internals;
  this->ProxyDefinitionManager->Delete();
  this->ProxyDefinitionManager = NULL;
}

//----------------------------------------------------------------------------
vtkPMObject* vtkSMSessionCore::GetPMObject(vtkTypeUInt32 globalid)
{
  return this->Internals->GetPMObject(globalid);
}

//----------------------------------------------------------------------------
void vtkSMSessionCore::PushStateInternal(vtkSMMessage* message)
{
  // When the control reaches here, we are assured that the PMObject needs be
  // created/exist on the local process.
  vtkPMObject* obj = this->Internals->GetPMObject(message->global_id());
  if (!obj)
    {
    if (!message->HasExtension(DefinitionHeader::server_class))
      {
      vtkErrorMacro("Message missing DefinitionHeader."
        "Aborting for debugging purposes.");
      abort();
      }
    // Create the corresponding PM object.
    vtkstd::string classname = message->GetExtension(DefinitionHeader::server_class);
    vtkSmartPointer<vtkObject> object;
    object.TakeReference(vtkInstantiator::CreateInstance(classname.c_str()));
    if (!object)
      {
      vtkErrorMacro("Failed to instantiate " << classname.c_str());
      abort();
      }
    obj = vtkPMObject::SafeDownCast(object);
    if (obj == NULL)
      {
      vtkErrorMacro("Object must be a vtkPMObject subclass. "
        "Aborting for debugging purposes.");
      abort();
      }
    obj->Initialize(this);
    this->Internals->PMObjectMap[message->global_id()] = obj;
    }

  // Push the message to the PMObject.
  obj->Push(message);
}

//----------------------------------------------------------------------------
void vtkSMSessionCore::PushState(vtkSMMessage* message)
{
  // This can only be called on the root node.
  assert(this->ParallelController == NULL ||
    this->ParallelController->GetLocalProcessId() == 0);


  if ( (message->location() & vtkProcessModule2::SERVERS) != 0)
    {
    // send message to satellites and then start processing.

    if (this->ParallelController &&
      this->ParallelController->GetNumberOfProcesses() > 1 &&
      this->ParallelController->GetLocalProcessId() == 0)
      {
      // Forward the message to the satellites if the object is expected to exist
      // on the satellites.

      // FIXME: There's one flaw in this logic. If a object is to be created on
      // DATA_SERVER_ROOT, but on all RENDER_SERVER nodes, then in render-server
      // configuration, the message will end up being send to all data-server
      // nodes as well. Although we never do that presently, it's a possibility
      // and we should fix this.
      unsigned char type = PUSH_STATE;
      this->ParallelController->TriggerRMIOnAllChildren(&type, 1,
        ROOT_SATELLITE_RMI_TAG);

      int byte_size = message->ByteSize();
      unsigned char *raw_data = new unsigned char[byte_size + 1];
      message->SerializeToArray(raw_data, byte_size);
      this->ParallelController->Broadcast(&byte_size, 1, 0);
      this->ParallelController->Broadcast(raw_data, byte_size, 0);
      delete [] raw_data;
      }
    }

  // When the control reaches here, we are assured that the PMObject needs be
  // created/exist on the local process.
  this->PushStateInternal(message);
}

//----------------------------------------------------------------------------
void vtkSMSessionCore::PushStateSatelliteCallback()
{
  cout << "PushStateSatelliteCallback" << endl;
  int byte_size = 0;
  this->ParallelController->Broadcast(&byte_size, 1, 0);

  unsigned char *raw_data = new unsigned char[byte_size + 1];
  this->ParallelController->Broadcast(raw_data, byte_size, 0);

  vtkSMMessage message;
  if (!message.ParseFromArray(raw_data, byte_size))
    {
    vtkErrorMacro("Failed to parse protobuf message.");
    }
  else
    {
    this->PushStateInternal(&message);
    }
  delete [] raw_data;
}

//----------------------------------------------------------------------------
void vtkSMSessionCore::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
//----------------------------------------------------------------------------
void vtkSMSessionCore::PullState(vtkSMMessage* message)
{
  vtkPMObject* obj;
  if(true &&  // FIXME make sure that the PMObject should be created here
     (obj = this->Internals->GetPMObject(message->global_id())))
    {
    obj->Pull(message);
    }
}

//----------------------------------------------------------------------------
void vtkSMSessionCore::Invoke(vtkSMMessage* message)
{
  vtkPMObject* obj;
  if(true &&  // FIXME make sure that the PMObject should be created here
     (obj = this->Internals->GetPMObject(message->global_id())))
    {
    obj->Invoke(message);
    }
}
//----------------------------------------------------------------------------
void vtkSMSessionCore::DeletePMObject(vtkSMMessage* message)
{
  this->Internals->Delete(message->global_id());
}


//----------------------------------------------------------------------------
bool vtkSMSessionCore::GatherInformationInternal(
  vtkPVInformation* information, vtkTypeUInt32 globalid)
{
  // default is to gather information from VTKObject, if FromPMObject is true,
  // then gather from PMObject.
  vtkPMObject* pmobject = this->GetPMObject(globalid);
  if (!pmobject)
    {
    vtkErrorMacro("No object with global-id: " << globalid);
    return false;
    }

  vtkPMProxy* pmproxy = vtkPMProxy::SafeDownCast(pmobject);
  if (pmproxy /*&& !information->GetUsePMObject()*/)
    {
    vtkObject* object = vtkObject::SafeDownCast(pmproxy->GetVTKObject());
    information->CopyFromObject(object);
    }
  else
    {
    // gather information from PMObject itself.
    information->CopyFromObject(pmobject);
    }
  return true;
}

//----------------------------------------------------------------------------
bool vtkSMSessionCore::GatherInformation(vtkTypeUInt32 location,
  vtkPVInformation* information, vtkTypeUInt32 globalid)
{
  // This can only be called on the root node.
  assert(this->ParallelController == NULL ||
    this->ParallelController->GetLocalProcessId() == 0);

  if (!this->GatherInformationInternal(information, globalid))
    {
    return false;
    }

  if (information->GetRootOnly() || (location & vtkProcessModule2::SERVERS) == 0)
    {
    return true;
    }

  // send message to satellites and then start processing.

  if (this->ParallelController &&
    this->ParallelController->GetNumberOfProcesses() > 1 &&
    this->ParallelController->GetLocalProcessId() == 0)
    {
    // Forward the message to the satellites if the object is expected to exist
    // on the satellites.

    // FIXME: There's one flaw in this logic. If a object is to be created on
    // DATA_SERVER_ROOT, but on all RENDER_SERVER nodes, then in render-server
    // configuration, the message will end up being send to all data-server
    // nodes as well. Although we never do that presently, it's a possibility
    // and we should fix this.
    unsigned char type = GATHER_INFORMATION;
    this->ParallelController->TriggerRMIOnAllChildren(&type, 1,
      ROOT_SATELLITE_RMI_TAG);

    vtkMultiProcessStream stream;
    stream << information->GetClassName() << globalid;
    this->ParallelController->Broadcast(stream, 0);
    }

  return this->CollectInformation(information);
}

//----------------------------------------------------------------------------
void vtkSMSessionCore::GatherInformationStatelliteCallback()
{
  vtkMultiProcessStream stream;
  this->ParallelController->Broadcast(stream, 0);

  vtkstd::string classname;
  vtkTypeUInt32 globalid;
  stream >> classname >> globalid;

  vtkSmartPointer<vtkObject> o;
  o.TakeReference(vtkInstantiator::CreateInstance(classname.c_str()));

  vtkPVInformation* info = vtkPVInformation::SafeDownCast(o);
  if (info)
    {
    this->GatherInformationInternal(info, globalid);
    this->CollectInformation(info);
    }
  else
    {
    vtkErrorMacro("Could not gather information on Satellite.");
    // let the parent know, otherwise root will hang.
    this->CollectInformation(NULL);
    }
}

//----------------------------------------------------------------------------
bool vtkSMSessionCore::CollectInformation(vtkPVInformation* info)
{
  vtkMultiProcessController* controller = this->ParallelController;
  int myid = controller->GetLocalProcessId();
  int numProcs = controller->GetNumberOfProcesses();

  int children[2] = {2*myid + 1, 2*myid + 2};
  int parent = myid > 0? (myid-1)/2 : -1;

  // General rule is: receive from children and send to parent
  for (int childno=0; childno < 2; childno++)
    {
    int childid = children[childno];
    if (childid >= numProcs)
      {
      // Skip nonexistent children.
      continue;
      }

    int length;
    controller->Receive(&length, 1, childid, ROOT_SATELLITE_INFO_TAG);
    if (length <= 0)
      {
      vtkErrorMacro(
        "Failed to Gather Information from satellite no: " << childid);
      continue;
      }

    unsigned char* data = new unsigned char[length];
    controller->Receive(data, length, childid, ROOT_SATELLITE_INFO_TAG);
    vtkClientServerStream stream;
    stream.SetData(data, length);
    vtkPVInformation* tempInfo = info->NewInstance();
    tempInfo->CopyFromStream(&stream);
    info->AddInformation(tempInfo);
    tempInfo->Delete();
    delete [] data;
    }

  // Now send to parent, if parent is indeed valid.
  if (parent >= 0)
    {
    if (info)
      {
      vtkClientServerStream css;
      info->CopyToStream(&css);
      size_t length;
      const unsigned char* data;
      css.GetData(&data, &length);
      int len = static_cast<int>(length);
      controller->Send(&len, 1, parent, ROOT_SATELLITE_INFO_TAG);
      controller->Send(const_cast<unsigned char*>(data),
        length, parent, ROOT_SATELLITE_INFO_TAG);
      }
    else
      {
      int len = 0;
      controller->Send(&len, 1, parent, ROOT_SATELLITE_INFO_TAG);
      }
    }
  return true;
}