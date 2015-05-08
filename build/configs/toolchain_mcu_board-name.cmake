# Copyright 2015 Samsung Electronics Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include(CMakeForceCompiler)

set(CMAKE_SYSTEM_NAME MCU)
set(CMAKE_SYSTEM_PROCESSOR <CPUARCH> (example: armv7l))
set(CMAKE_SYSTEM_VERSION <BOARDNAME> (example: STM32F4))

set(FLAGS_COMMON_ARCH <ARCHITECTUREFLAGS> (example: -mlittle-endian -mthumb -mcpu=cortex-m4 -march=armv7e-m -mfpu=fpv4-sp-d16 -mfloat-abi=hard))

CMAKE_FORCE_C_COMPILER(<C_COMPILER> (example: arm-none-eabi-gcc) GNU)
CMAKE_FORCE_CXX_COMPILER(<CPP_COMPILER> (example: arm-none-eabi-g++) GNU)
