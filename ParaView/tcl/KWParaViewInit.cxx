#include "vtkTclUtil.h"
int vtkDummyRenderWindowInteractorCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkDummyRenderWindowInteractorNewCommand();
int vtkImageOutlineFilterCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkImageOutlineFilterNewCommand();
int vtkInteractorStyleCameraCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkInteractorStyleCameraNewCommand();
int vtkInteractorStyleGridExtentCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkInteractorStyleGridExtentNewCommand();
int vtkInteractorStyleImageExtentCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkInteractorStyleImageExtentNewCommand();
int vtkInteractorStylePlaneCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkInteractorStylePlaneNewCommand();
int vtkInteractorStylePlaneSourceCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkInteractorStylePlaneSourceNewCommand();
int vtkInteractorStyleSphereCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkInteractorStyleSphereNewCommand();
int vtkKWRenderViewCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkKWRenderViewNewCommand();
int vtkPVApplicationCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVApplicationNewCommand();
int vtkPVAssignmentCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVAssignmentNewCommand();
int vtkPVConeSourceCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVConeSourceNewCommand();
int vtkPVContourFilterCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVContourFilterNewCommand();
int vtkPVDataCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVDataNewCommand();
int vtkPVElevationFilterCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVElevationFilterNewCommand();
int vtkPVGlyph3DCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVGlyph3DNewCommand();
int vtkPVImageCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVImageNewCommand();
int vtkPVImageClipCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVImageClipNewCommand();
int vtkPVImageReaderCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVImageReaderNewCommand();
int vtkPVImageSliceCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVImageSliceNewCommand();
int vtkPVImageTextureFilterCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVImageTextureFilterNewCommand();
int vtkPVMenuButtonCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVMenuButtonNewCommand();
int vtkPVPolyDataCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVPolyDataNewCommand();
int vtkPVRenderViewCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVRenderViewNewCommand();
int vtkPVShrinkPolyDataCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVShrinkPolyDataNewCommand();
int vtkPVSourceCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVSourceNewCommand();
int vtkPVSourceListCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVSourceListNewCommand();
int vtkPVWindowCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkPVWindowNewCommand();

extern Tcl_HashTable vtkInstanceLookup;
extern Tcl_HashTable vtkPointerLookup;
extern Tcl_HashTable vtkCommandLookup;
extern void vtkTclDeleteObjectFromHash(void *);
extern void vtkTclListInstances(Tcl_Interp *interp, ClientData arg);


extern "C" {int VTK_EXPORT Vtkkwparaviewtcl_SafeInit(Tcl_Interp *interp);}



extern "C" {int VTK_EXPORT Vtkkwparaviewtcl_Init(Tcl_Interp *interp);}



extern void vtkTclGenericDeleteObject(ClientData cd);



int VTK_EXPORT Vtkkwparaviewtcl_SafeInit(Tcl_Interp *interp)
{
  return Vtkkwparaviewtcl_Init(interp);
}


int VTK_EXPORT Vtkkwparaviewtcl_Init(Tcl_Interp *interp)
{
  vtkTclCreateNew(interp,"vtkDummyRenderWindowInteractor", vtkDummyRenderWindowInteractorNewCommand,
                  vtkDummyRenderWindowInteractorCommand);
  vtkTclCreateNew(interp,"vtkImageOutlineFilter", vtkImageOutlineFilterNewCommand,
                  vtkImageOutlineFilterCommand);
  vtkTclCreateNew(interp,"vtkInteractorStyleCamera", vtkInteractorStyleCameraNewCommand,
                  vtkInteractorStyleCameraCommand);
  vtkTclCreateNew(interp,"vtkInteractorStyleGridExtent", vtkInteractorStyleGridExtentNewCommand,
                  vtkInteractorStyleGridExtentCommand);
  vtkTclCreateNew(interp,"vtkInteractorStyleImageExtent", vtkInteractorStyleImageExtentNewCommand,
                  vtkInteractorStyleImageExtentCommand);
  vtkTclCreateNew(interp,"vtkInteractorStylePlane", vtkInteractorStylePlaneNewCommand,
                  vtkInteractorStylePlaneCommand);
  vtkTclCreateNew(interp,"vtkInteractorStylePlaneSource", vtkInteractorStylePlaneSourceNewCommand,
                  vtkInteractorStylePlaneSourceCommand);
  vtkTclCreateNew(interp,"vtkInteractorStyleSphere", vtkInteractorStyleSphereNewCommand,
                  vtkInteractorStyleSphereCommand);
  vtkTclCreateNew(interp,"vtkKWRenderView", vtkKWRenderViewNewCommand,
                  vtkKWRenderViewCommand);
  vtkTclCreateNew(interp,"vtkPVApplication", vtkPVApplicationNewCommand,
                  vtkPVApplicationCommand);
  vtkTclCreateNew(interp,"vtkPVAssignment", vtkPVAssignmentNewCommand,
                  vtkPVAssignmentCommand);
  vtkTclCreateNew(interp,"vtkPVConeSource", vtkPVConeSourceNewCommand,
                  vtkPVConeSourceCommand);
  vtkTclCreateNew(interp,"vtkPVContourFilter", vtkPVContourFilterNewCommand,
                  vtkPVContourFilterCommand);
  vtkTclCreateNew(interp,"vtkPVData", vtkPVDataNewCommand,
                  vtkPVDataCommand);
  vtkTclCreateNew(interp,"vtkPVElevationFilter", vtkPVElevationFilterNewCommand,
                  vtkPVElevationFilterCommand);
  vtkTclCreateNew(interp,"vtkPVGlyph3D", vtkPVGlyph3DNewCommand,
                  vtkPVGlyph3DCommand);
  vtkTclCreateNew(interp,"vtkPVImage", vtkPVImageNewCommand,
                  vtkPVImageCommand);
  vtkTclCreateNew(interp,"vtkPVImageClip", vtkPVImageClipNewCommand,
                  vtkPVImageClipCommand);
  vtkTclCreateNew(interp,"vtkPVImageReader", vtkPVImageReaderNewCommand,
                  vtkPVImageReaderCommand);
  vtkTclCreateNew(interp,"vtkPVImageSlice", vtkPVImageSliceNewCommand,
                  vtkPVImageSliceCommand);
  vtkTclCreateNew(interp,"vtkPVImageTextureFilter", vtkPVImageTextureFilterNewCommand,
                  vtkPVImageTextureFilterCommand);
  vtkTclCreateNew(interp,"vtkPVMenuButton", vtkPVMenuButtonNewCommand,
                  vtkPVMenuButtonCommand);
  vtkTclCreateNew(interp,"vtkPVPolyData", vtkPVPolyDataNewCommand,
                  vtkPVPolyDataCommand);
  vtkTclCreateNew(interp,"vtkPVRenderView", vtkPVRenderViewNewCommand,
                  vtkPVRenderViewCommand);
  vtkTclCreateNew(interp,"vtkPVShrinkPolyData", vtkPVShrinkPolyDataNewCommand,
                  vtkPVShrinkPolyDataCommand);
  vtkTclCreateNew(interp,"vtkPVSource", vtkPVSourceNewCommand,
                  vtkPVSourceCommand);
  vtkTclCreateNew(interp,"vtkPVSourceList", vtkPVSourceListNewCommand,
                  vtkPVSourceListCommand);
  vtkTclCreateNew(interp,"vtkPVWindow", vtkPVWindowNewCommand,
                  vtkPVWindowCommand);
  return TCL_OK;
}
