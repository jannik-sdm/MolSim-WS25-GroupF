option(ENABLE_OPENMP "Enable OpenMP parallelization" ON)

if (ENABLE_OPENMP)
	find_package(OpenMP REQUIRED)

	if (OpenMP_FOUND)
		message(STATUS "OpenMP found, enabling support")

		target_link_libraries(MolSimLib OpenMP::OpenMP_CXX)
	else()
		message(FATAL_ERROR "OpenMP not found")
	endif()
endif()
