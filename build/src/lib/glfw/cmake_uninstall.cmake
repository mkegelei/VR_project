
<<<<<<< HEAD
if (NOT EXISTS "/home/panda-cool/openGL/VR_project/build/src/lib/glfw/install_manifest.txt")
  message(FATAL_ERROR "Cannot find install manifest: \"/home/panda-cool/openGL/VR_project/build/src/lib/glfw/install_manifest.txt\"")
endif()

file(READ "/home/panda-cool/openGL/VR_project/build/src/lib/glfw/install_manifest.txt" files)
=======
if (NOT EXISTS "/home/lucas/VR_project/build/src/lib/glfw/install_manifest.txt")
  message(FATAL_ERROR "Cannot find install manifest: \"/home/lucas/VR_project/build/src/lib/glfw/install_manifest.txt\"")
endif()

file(READ "/home/lucas/VR_project/build/src/lib/glfw/install_manifest.txt" files)
>>>>>>> 1e57215b089641099e79fd8f7b0b1f408357cc43
string(REGEX REPLACE "\n" ";" files "${files}")

foreach (file ${files})
  message(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
  if (EXISTS "$ENV{DESTDIR}${file}")
    exec_program("/usr/bin/cmake" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
                 OUTPUT_VARIABLE rm_out
                 RETURN_VALUE rm_retval)
    if (NOT "${rm_retval}" STREQUAL 0)
      MESSAGE(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
    endif()
  elseif (IS_SYMLINK "$ENV{DESTDIR}${file}")
    EXEC_PROGRAM("/usr/bin/cmake" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
                 OUTPUT_VARIABLE rm_out
                 RETURN_VALUE rm_retval)
    if (NOT "${rm_retval}" STREQUAL 0)
      message(FATAL_ERROR "Problem when removing symlink \"$ENV{DESTDIR}${file}\"")
    endif()
  else()
    message(STATUS "File \"$ENV{DESTDIR}${file}\" does not exist.")
  endif()
endforeach()

