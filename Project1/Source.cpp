#include <vtkSmartPointer.h>
#include <cmath>
#include <vtkGlyph3D.h>
#include <vtkLODActor.h>
#include <vtkLODActor.h>
#include <vtkPDBReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkColorTransferFunction.h>
#include <vtkConeSource.h>
#include <vtkProperty.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkTextProperty.h>
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
#include <vtkCubeAxesActor.h>
#include <vtkTextActor.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif

VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);
//VTK_MODULE_INIT(vtkActor);
//VTK_MODULE_INIT(vtkPolyDataMapper);


int main(int argc, char* argv[])
{

	// Setup the OpenVR rendering classes
	vtkNew<vtkActor> actor;
	vtkNew<vtkVolume> volume;
	vtkNew<vtkOpenVRRenderer> renderer;
	vtkNew<vtkOpenVRRenderWindow> renderWindow;
	vtkNew<vtkOpenVRRenderWindowInteractor> iren;
	vtkNew<vtkOpenVRInteractorStyle> style;
	vtkNew<vtkOpenVRCamera> cam;

	vtkSmartPointer<vtkPDBReader> pdb =
		vtkSmartPointer<vtkPDBReader>::New();
	pdb->SetFileName("C:\\Users\\Aditya Dua\\OneDrive\\InfoHub\\Uni\\MXB362\\Project\\pdb2vtk\\proteinDataBankFiles\\1hhoExtA.pdb");
	pdb->SetHBScale(1.0);
	pdb->SetBScale(1.0);
	pdb->Update();
	vtkPolyData* outputPolyData = pdb->GetOutput();

	std::cout << "# of atoms is: " << pdb->GetNumberOfAtoms() << std::endl;

	double resolution = std::sqrt(300000.0 / pdb->GetNumberOfAtoms());
	if (resolution > 20)
	{
		resolution = 20;
	}
	if (resolution < 4)
	{
		resolution = 4;
	}
	std::cout << "Resolution is: " << resolution << std::endl;


	double bounds[6];
	outputPolyData->GetBounds(bounds);
	// Find min and max z
	double minz = bounds[4];
	double maxz = bounds[5];

	std::cout << "minz: " << minz << std::endl;
	std::cout << "maxz: " << maxz << std::endl;
	vtkSmartPointer<vtkLookupTable> colorLookupTable =
		vtkSmartPointer<vtkLookupTable>::New();
	colorLookupTable->SetTableRange(minz, maxz);
	colorLookupTable->Build();
	
	// Generate the colors for each point based on the color map
	vtkSmartPointer<vtkUnsignedCharArray> colors =
		vtkSmartPointer<vtkUnsignedCharArray>::New();
	colors->SetNumberOfComponents(3);
	colors->SetName("Colors");

		for(int i = 0; i <outputPolyData->GetNumberOfPoints(); i++)
		{
			double p[3];
			outputPolyData->GetPoint(i, p);

			double dcolor[3];
			colorLookupTable->GetColor(p[2], dcolor);
			std::cout << "dcolor: "
				<< dcolor[0] << " "
				<< dcolor[1] << " "
				<< dcolor[2] << std::endl;
			unsigned char color[3];
			for (unsigned int j = 0; j < 3; j++)
			{
				color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);
			}
			std::cout << "color: "
				<< (int)color[0] << " "
				<< (int)color[1] << " "
				<< (int)color[2] << std::endl;

			colors->InsertNextTupleValue(color);
		}

	outputPolyData->GetPointData()->SetScalars(colors);

	vtkSmartPointer<vtkSphereSource> sphere =
		vtkSmartPointer<vtkSphereSource>::New();
	sphere->SetCenter(0, 0, 0);
	sphere->SetRadius(1);
	sphere->SetThetaResolution(static_cast<int>(resolution));
	sphere->SetPhiResolution(static_cast<int>(resolution));

	vtkSmartPointer<vtkGlyph3D> glyph =
		vtkSmartPointer<vtkGlyph3D>::New();
	glyph->SetInputConnection(pdb->GetOutputPort());
	glyph->SetOrient(1);
	glyph->SetColorMode(1);
	// glyph->ScalingOn();
	glyph->SetScaleMode(2);
	glyph->SetScaleFactor(.25);
	glyph->SetSourceConnection(sphere->GetOutputPort());

	vtkSmartPointer<vtkPolyDataMapper> atomMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	atomMapper->SetInputConnection(glyph->GetOutputPort());
	atomMapper->ImmediateModeRenderingOn();
	atomMapper->UseLookupTableScalarRangeOff();
	atomMapper->ScalarVisibilityOn();
	atomMapper->SetScalarModeToDefault();


	vtkSmartPointer<vtkLODActor> atom =
		vtkSmartPointer<vtkLODActor>::New();
	atom->SetMapper(atomMapper);
	atom->GetProperty()->SetRepresentationToSurface();
	atom->GetProperty()->SetInterpolationToGouraud();
	atom->GetProperty()->SetAmbient(0.15);
	atom->GetProperty()->SetDiffuse(0.85);
	atom->GetProperty()->SetSpecular(0.1);
	atom->GetProperty()->SetSpecularPower(30);
	atom->GetProperty()->SetSpecularColor(1, 1, 1);
	atom->SetNumberOfCloudPoints(30000);

	renderer->AddActor(atom);

	vtkSmartPointer<vtkTubeFilter> tube =
		vtkSmartPointer<vtkTubeFilter>::New();
	tube->SetInputConnection(pdb->GetOutputPort());
	tube->SetNumberOfSides(static_cast<int>(resolution));
	tube->CappingOff();
	tube->SetRadius(0.2);
	tube->SetVaryRadius(0);
	tube->SetRadiusFactor(10);

	vtkSmartPointer<vtkPolyDataMapper> bondMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	bondMapper->SetInputConnection(tube->GetOutputPort());
	bondMapper->ImmediateModeRenderingOn();
	bondMapper->UseLookupTableScalarRangeOff();
	bondMapper->ScalarVisibilityOff();
	bondMapper->SetScalarModeToDefault();

	vtkSmartPointer<vtkLODActor> bond =
		vtkSmartPointer<vtkLODActor>::New();
	bond->SetMapper(bondMapper);
	bond->GetProperty()->SetRepresentationToSurface();
	bond->GetProperty()->SetInterpolationToGouraud();
	bond->GetProperty()->SetAmbient(0.15);
	bond->GetProperty()->SetDiffuse(0.85);
	bond->GetProperty()->SetSpecular(0.1);
	bond->GetProperty()->SetSpecularPower(30);
	bond->GetProperty()->SetSpecularColor(1, 1, 1);
	bond->GetProperty()->SetDiffuseColor(1.0000, 0.8941, 0.70981);

	renderer->AddActor(bond);

	renderWindow->AddRenderer(renderer.Get());
	iren->SetRenderWindow(renderWindow.Get());
	iren->SetInteractorStyle(style.Get());
	renderer->SetActiveCamera(cam.Get());

	vtkSmartPointer<vtkCubeAxesActor> cubeAxesActor = vtkSmartPointer<vtkCubeAxesActor>::New();
	cubeAxesActor->SetCamera(renderer->GetActiveCamera());
	cubeAxesActor->SetBounds(-15, 40, -15, 40, -40, 10);
	cubeAxesActor->GetTitleTextProperty(0)->SetColor(1.0, 0.0, 0.0);
	cubeAxesActor->GetLabelTextProperty(0)->SetColor(1.0, 0.0, 0.0);

	cubeAxesActor->GetTitleTextProperty(1)->SetColor(0.0, 1.0, 0.0);
	cubeAxesActor->GetLabelTextProperty(1)->SetColor(0.0, 1.0, 0.0);

	cubeAxesActor->GetTitleTextProperty(2)->SetColor(0.0, 0.0, 1.0);
	cubeAxesActor->GetLabelTextProperty(2)->SetColor(0.0, 0.0, 1.0);
	cubeAxesActor->SetFlyModeToStaticTriad();
	renderer->AddActor(cubeAxesActor);
	
	// Without the next line volume rendering in VR does not work
	renderWindow->SetMultiSamples(0);
	
	double *view = (double *)malloc(sizeof(double)*3);
	// Render
	renderer->ResetCamera();
	renderWindow->Render();
	view = iren->GetPhysicalTranslation(cam);
	iren->SetPhysicalTranslation(cam, -5, -5, -60);
	printf("%f, %f, %f\n", *view, *(view + 1), *(view + 2));
	iren->Start();

	return EXIT_SUCCESS;
}