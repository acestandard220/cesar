cmake_minimum_required(VERSION 3.30)

cmake_policy(SET CMP0054 NEW)

project(PixCapturePlayer)

# This makes sure that CMake will look for the latest available Windows SDK Version.
set(CMAKE_SYSTEM_VERSION 10.0)
set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(USE_WINPIX_EVENT_RUNTIME true)
set(USE_D3D12_AGILITY_SDK true)

add_executable(PixCapturePlayer WIN32)
target_sources(PixCapturePlayer PRIVATE
    Main.cpp
    pch.cpp
    CapturedAssets.cpp
    Helpers.h
    RenderFrame.cpp
    Win32Application.cpp
    ResourceReader.cpp
    UploadBuffers.cpp
    FrameResources.cpp
    CreatePSOs.cpp
    ResourceModifications.h
    CreateAndInitResources_000.cpp
    CommandLists_000.cpp
    RenderFrameWorker_000.cpp
    ShaderTableReconstruction_000.cpp
    AccelStructureRecreation_000.cpp
    RaytracingInstanceDescs_000.cpp
    Descriptors_000.cpp
    ModifyDescriptors_000.cpp
    ResourceModifications_000.cpp
    FrameResources_000.cpp
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PixCapturePlayer)

if (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "/ZI")
endif()

target_compile_options(PixCapturePlayer
    PRIVATE
        # /ZI produces a PDB file in a format that supports the Edit and Continue feature
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/ZI>
        $<$<CXX_COMPILER_ID:MSVC>:
            -bigobj     # Increases the number of sections that an object file can contain
            -GS-        # Disable buffer security check
            -MP         # Builds multiple source files concurrently
        >
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>:
            -bigobj     # Increases the number of sections that an object file can contain
            -GS-        # Disable buffer security check
            -Wno-unused-function
            -Wno-missing-braces
            -Wno-microsoft-cast
        >
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>>:
            -g          # Generate source level debug information
            -Wno-unused-function
            -Wno-missing-braces
            -Wno-microsoft-cast
        >
)

# Use generator expressions for link options (stack size)
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.13.0")
    target_link_options(PixCapturePlayer
        PRIVATE
            $<$<OR:$<CXX_COMPILER_ID:MSVC>,$<AND:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>>:/STACK:10000000>
    )
endif()

set(WINPIX_EVENT_RUNTIME_DLL ${CMAKE_CURRENT_SOURCE_DIR}/WinPixEventRuntime/bin/x64/WinPixEventRuntime.dll)
set(WINPIX_EVENT_RUNTIME_LIB ${CMAKE_CURRENT_SOURCE_DIR}/WinPixEventRuntime/bin/x64/WinPixEventRuntime.lib)

if (WINPIX_EVENT_RUNTIME_ARM64)

    set(WINPIX_EVENT_RUNTIME_DLL ${CMAKE_CURRENT_SOURCE_DIR}/WinPixEventRuntime/bin/ARM64/WinPixEventRuntime.dll)
    set(WINPIX_EVENT_RUNTIME_LIB ${CMAKE_CURRENT_SOURCE_DIR}/WinPixEventRuntime/bin/ARM64/WinPixEventRuntime.lib)

endif()

set(WINPIX_EVENT_RUNTIME_NUPKG_NAME WinPixEventRuntime.nupkg)
set(WINPIX_EVENT_RUNTIME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/WinPixEventRuntime)
set(INTERNET_CONNECTION_AVAILABLE true)
set(DX12_AGILITY_SDK_NUPKG_NAME D3D12AgilitySdk.nupkg)
set(AGILITY_SDK_EXTRACT_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/AgilitySDK")
set(AGILITY_SDK_PATH ${AGILITY_SDK_EXTRACT_FOLDER})

target_compile_definitions(PixCapturePlayer PRIVATE -RESOURCES_BIN_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources.bin")

# Check for internet connection
if (USE_D3D12_AGILITY_SDK OR USE_WINPIX_EVENT_RUNTIME)

    execute_process(
        COMMAND curl -I https://bing.com --connect-timeout 5
        ERROR_QUIET
        RESULT_VARIABLE CONNECTION_STATUS
    )

	if (CONNECTION_STATUS GREATER 0)

		message(WARNING "No internet connection")
		set(INTERNET_CONNECTION_AVAILABLE false)

	endif()

endif()

if (USE_D3D12_AGILITY_SDK)

    if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${DX12_AGILITY_SDK_NUPKG_NAME})

        if (INTERNET_CONNECTION_AVAILABLE)

            message(INFO "Downloading DirectX 12 Agility SDK nuget package...")

	        file(DOWNLOAD
		        https://www.nuget.org/api/v2/package/Microsoft.Direct3D.D3D12${AGILITY_SDK_DOWNLOAD_POSTFIX}
		        ${CMAKE_CURRENT_SOURCE_DIR}/${DX12_AGILITY_SDK_NUPKG_NAME}
                STATUS DOWNLOAD_STATUS
	        )

            list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
            list(GET DOWNLOAD_STATUS 1 ERROR_MESSAGE)

            if (${STATUS_CODE} EQUAL 0)

                message(STATUS "Agility SDK download complete!")

            else()

                message(WARNING "Download was unsuccessful. Error: ${ERROR_MESSAGE}")

            endif()

	        if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${DX12_AGILITY_SDK_NUPKG_NAME})

		        message(WARNING "Could not download DirectX 12 Agility SDK nuget package. Project will load system D3D12Core.dll instead.")

            endif()

        else()

            message(WARNING "No internet connection. Cannot download DirectX 12 Agility SDK nuget package. Project will load system D3D12Core.dll instead.")

        endif()

    endif()

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${DX12_AGILITY_SDK_NUPKG_NAME})

        message(STATUS "Extracting Agility SDK nuget package contents!")

		# Convert the file from .nupkg to .zip to extract its contents
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${DX12_AGILITY_SDK_NUPKG_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/D3D12AgilitySdk.zip COPYONLY)

		file(MAKE_DIRECTORY  ${AGILITY_SDK_EXTRACT_FOLDER})

		# Extract contents from .zip file to created directory
		execute_process(
			COMMAND ${CMAKE_COMMAND} "-E" "tar" "xvz" "${CMAKE_CURRENT_SOURCE_DIR}/D3D12AgilitySdk.zip"
			WORKING_DIRECTORY ${AGILITY_SDK_EXTRACT_FOLDER}
		)

		if (WINPIX_EVENT_RUNTIME_ARM64)

			set(AGILITY_SDK_PATH "${AGILITY_SDK_EXTRACT_FOLDER}/build/native/bin/arm64")

		else()

			set(AGILITY_SDK_PATH "${AGILITY_SDK_EXTRACT_FOLDER}/build/native/bin/x64")

		endif()

		add_custom_command(TARGET PixCapturePlayer POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory
			${AGILITY_SDK_PATH}
			$<TARGET_FILE_DIR:PixCapturePlayer>/D3D12
			COMMENT "Copying Agility SDK files to /D3D12 that is next to .exe"
		)

        target_compile_definitions(PixCapturePlayer PRIVATE AGILITY_SDK)

    endif()

endif()

if (USE_WINPIX_EVENT_RUNTIME)

    # If WinPixEventRuntime is not downloaded already, download it.
    if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${WINPIX_EVENT_RUNTIME_NUPKG_NAME})

        if (INTERNET_CONNECTION_AVAILABLE)

            message(INFO "Downloading WinPixEventRuntime.nupkg...")

	        file(DOWNLOAD
		        https://www.nuget.org/api/v2/package/WinPixEventRuntime
		        ${CMAKE_CURRENT_SOURCE_DIR}/${WINPIX_EVENT_RUNTIME_NUPKG_NAME}
                STATUS DOWNLOAD_STATUS
	        )

            list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
            list(GET DOWNLOAD_STATUS 1 ERROR_MESSAGE)

            if (${STATUS_CODE} EQUAL 0)

                message(STATUS "WinPixEventRuntime.nupkg download complete!")

            else()

                message(WARNING "Download was unsuccessful. Error: ${ERROR_MESSAGE}")

            endif()

	        if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${WINPIX_EVENT_RUNTIME_NUPKG_NAME})

		        message(WARNING "Could not download WinPixEventRuntime nuget package. Project will not use WinPixEventRuntime library")

	        endif()

        else()

            message(WARNING "No internet connection. Cannot download WinPixEventRuntime nuget package. Project will not use WinPixEventRuntime library")

        endif()

    endif()

endif()

if (USE_WINPIX_EVENT_RUNTIME AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${WINPIX_EVENT_RUNTIME_NUPKG_NAME})

    message(STATUS "Extracting WinPixEventRuntime.nupkg contents!")

    # Convert the file from .nupkg to .zip to extract its contents
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${WINPIX_EVENT_RUNTIME_NUPKG_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/WinPixEventRuntime.zip COPYONLY)

    file(MAKE_DIRECTORY  ${WINPIX_EVENT_RUNTIME_DIR})

    # Extract contents from .zip file to created directory
    execute_process(
        COMMAND ${CMAKE_COMMAND} "-E" "tar" "xvz" "${CMAKE_CURRENT_SOURCE_DIR}/WinPixEventRuntime.zip"
        WORKING_DIRECTORY ${WINPIX_EVENT_RUNTIME_DIR}
    )

    add_custom_command(TARGET PixCapturePlayer
		COMMAND ${CMAKE_COMMAND} -E copy
		${WINPIX_EVENT_RUNTIME_DLL}
		$<TARGET_FILE_DIR:PixCapturePlayer>/WinPixEventRuntime.dll
		COMMENT "Copying WinPixEventRuntime.dll next to PixCapturePlayer.exe"
	)
	target_link_libraries(PixCapturePlayer
		d3d12.lib
		dxgi.lib
		d3dcompiler.lib
		dxguid.lib
		cabinet.lib
		${WINPIX_EVENT_RUNTIME_LIB}
	)

    target_compile_definitions(PixCapturePlayer PRIVATE WIN_PIX_EVENT_RUNTIME)

else()

    target_link_libraries(PixCapturePlayer
        d3d12.lib
        dxgi.lib
        d3dcompiler.lib
        dxguid.lib
        cabinet.lib)

endif()

# Installing targets
install(TARGETS PixCapturePlayer RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "cesar")
