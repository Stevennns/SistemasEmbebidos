# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/express-idf/components/bootloader/subproject"
  "D:/Universidad/2023-1/Sistemas embedidos/Tarea1/sample_project/build/bootloader"
  "D:/Universidad/2023-1/Sistemas embedidos/Tarea1/sample_project/build/bootloader-prefix"
  "D:/Universidad/2023-1/Sistemas embedidos/Tarea1/sample_project/build/bootloader-prefix/tmp"
  "D:/Universidad/2023-1/Sistemas embedidos/Tarea1/sample_project/build/bootloader-prefix/src/bootloader-stamp"
  "D:/Universidad/2023-1/Sistemas embedidos/Tarea1/sample_project/build/bootloader-prefix/src"
  "D:/Universidad/2023-1/Sistemas embedidos/Tarea1/sample_project/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Universidad/2023-1/Sistemas embedidos/Tarea1/sample_project/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Universidad/2023-1/Sistemas embedidos/Tarea1/sample_project/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
