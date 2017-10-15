#include <vtkSmartPointer.h>
#include <cmath>
#include <vtkGlyph3D.h>
#include <vtkLODActor.h>
#include <vtkLODActor.h>
#include <vtkPDBReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkActor.h>
#include <vtkColorTransferFunction.h>
#include <vtkConeSource.h>
#include <vtkProperty.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkNew.h>
#include <vtkOpenVRCamera.h>
#include <vtkOpenVRInteractorStyle.h>
#include <vtkOpenVRRenderer.h>
#include <vtkOpenVRRenderWindow.h>
#include <vtkOpenVRRenderWindowInteractor.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkSphereSource.h>
#include <vtkTubeFilter.h>
#include <vtkCommand.h>
#include <vtkEventData.h>
#include <vtkAutoInit.h>
#include <vtkGenericDataObjectReader.h>

VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
//VTK_MODULE_INIT(vtkActor);
//VTK_MODULE_INIT(vtkPolyDataMapper);


int main(int argc, char* argv[])
{

	// Setup the OpenVR rendering classes
	vtkNew<vtkActor> actor;
	vtkNew<vtkOpenVRRenderer> renderer;
	vtkNew<vtkOpenVRRenderWindow> renderWindow;
	vtkNew<vtkOpenVRRenderWindowInteractor> iren;
	vtkNew<vtkOpenVRInteractorStyle> style;
	vtkNew<vtkOpenVRCamera> cam;

	// simply set filename here (oh static joy)
	std::string inputFilename = "C:\\Users\\Aditya Dua\\OneDrive\\InfoHub\\Uni\\MXB362\\Project\\pdb2vtk\\proteinDataBankFiles\\1hhoExtA.pdb";

	// Get all data from the file
	vtkSmartPointer<vtkXMLPolyDataReader> reader =
		vtkSmartPointer<vtkXMLPolyDataReader>::New();
	reader->SetFileName(inputFilename.c_str());
	reader->Update();

	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(reader->GetOutputPort());

	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	//actor->SetMapper(mapper);
	actor->SetMapper(mapper);
	renderer->AddActor(actor);


	renderWindow->AddRenderer(renderer.Get());
	iren->SetRenderWindow(renderWindow.Get());
	iren->SetInteractorStyle(style.Get());
	renderer->SetActiveCamera(cam.Get());

	// Without the next line volume rendering in VR does not work
	renderWindow->SetMultiSamples(0);

	// Render
	renderer->ResetCamera();
	renderWindow->Render();
	iren->Start();

	return EXIT_SUCCESS;

}