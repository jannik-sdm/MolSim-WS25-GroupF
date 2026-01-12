#pragma once
#ifdef ENABLE_VTK_OUTPUT

#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>

#include <iomanip>
#include <sstream>
#include <string>

namespace outputWriter {

class VTKWriter {
 public:
  VTKWriter() = default;
  ~VTKWriter() = default;

  /**
   * @brief Generic plot method.
   * @tparam Function Lambda signature: (vtkPoints*, vtkFloatArray* mass, vtkFloatArray* velocity, vtkFloatArray* force,
   * vtkIntArray* type)
   */
  template <typename Function>
  void plotParticles(Function filler, const std::string &filename, int iteration) {
    // 1. Setup VTK Arrays
    auto points = vtkSmartPointer<vtkPoints>::New();

    vtkNew<vtkFloatArray> massArray;
    massArray->SetName("mass");
    massArray->SetNumberOfComponents(1);

    vtkNew<vtkFloatArray> velocityArray;
    velocityArray->SetName("velocity");
    velocityArray->SetNumberOfComponents(3);

    vtkNew<vtkFloatArray> forceArray;
    forceArray->SetName("force");
    forceArray->SetNumberOfComponents(3);

    vtkNew<vtkIntArray> typeArray;
    typeArray->SetName("type");
    typeArray->SetNumberOfComponents(1);

    // 2. Call the lambda to fill arrays (Inversion of Control)
    // We pass raw pointers (.Get()) for easy access in the lambda
    filler(points.Get(), massArray.Get(), velocityArray.Get(), forceArray.Get(), typeArray.Get());

    // 3. Setup Grid and Write
    auto grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    grid->SetPoints(points);
    grid->GetPointData()->AddArray(massArray);
    grid->GetPointData()->AddArray(velocityArray);
    grid->GetPointData()->AddArray(forceArray);
    grid->GetPointData()->AddArray(typeArray);

    std::stringstream strstr;
    strstr << filename << "_" << std::setfill('0') << std::setw(4) << iteration << ".vtu";

    vtkNew<vtkXMLUnstructuredGridWriter> writer;
    writer->SetFileName(strstr.str().c_str());
    writer->SetInputData(grid);
    writer->SetDataModeToAscii();
    writer->Write();
  }
};
}  // namespace outputWriter
#endif