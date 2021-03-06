set(SourceFiles
  install.sh.in

  Resources/Images.xcassets
  Info.plist.in
  MainMenu.xib

  QuipApplicationDelegate.h
  QuipApplicationDelegate.mm
  QuipDocument.h
  QuipDocument.mm
  QuipDocumentController.h
  QuipDocumentController.mm
  QuipPopupView.h
  QuipPopupView.mm
  QuipStatusView.h
  QuipStatusView.mm
  QuipTextView.h
  QuipTextView.mm
  QuipWindowController.h
  QuipWindowController.mm
  QuipWindowController.xib
  main.m
)
source_group(Code FILES ${SourceFiles})

set(ServiceSourceFiles
  DrawingServiceProvider.hpp
  DrawingServiceProvider.mm
  PopupServiceProvider.hpp
  PopupServiceProvider.mm
  StatusServiceProvider.hpp
  StatusServiceProvider.mm
)
source_group(Code\\Services FILES ${ServiceSourceFiles})

set(RuntimeFiles
  Runtime/boot.lua
  Runtime/syntax.lua
)
set(SyntaxFiles
  Runtime/syntax/cpp.lua
  Runtime/syntax/glsl.lua
  Runtime/syntax/markdown.lua
  Runtime/syntax/text.lua
)
source_group(Runtime FILES ${RuntimeFiles} ${SyntaxFiles})

set(RESOURCE
  Resources/Images.xcassets
)

add_executable(Quip MACOSX_BUNDLE ${SourceFiles} ${ServiceSourceFiles} ${RuntimeFiles} ${SyntaxFiles})

add_dependencies(Quip LPeg)
add_custom_command(TARGET Quip POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:LPeg> $<TARGET_FILE_DIR:Quip>/../Resources/lpeg.so
)

set_target_properties(Quip PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED ON)
set_target_properties(Quip PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/Info.plist.in)

target_compile_options(Quip PRIVATE "-fobjc-arc")

target_include_directories(Quip PRIVATE ../../Dependencies/lua/include)
target_include_directories(Quip PRIVATE ../../Dependencies/optional-lite)
target_include_directories(Quip PRIVATE ../Core)

target_link_libraries(Quip PRIVATE Quip.Core)
target_link_libraries(Quip PRIVATE ${CocoaFramework} ${CoreGraphicsFramework})

set_target_properties(Quip PROPERTIES RESOURCE Resources/Images.xcassets)
set_target_properties(Quip PROPERTIES XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon)

set_source_files_properties(${RuntimeFiles} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/Runtime)
set_source_files_properties(${SyntaxFiles} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/Runtime/syntax)
set_source_files_properties(MainMenu.xib QuipWindowController.xib PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

configure_file(install.sh.in ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/install.sh NEWLINE_STYLE UNIX)
file(COPY ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/install.sh DESTINATION ${PROJECT_BINARY_DIR}
  FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ
)
