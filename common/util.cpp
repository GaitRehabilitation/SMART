/**
* Copyright 2019 GaitRehabilitation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "util.h"

#include <stdexcept>

uint64_t Util::stringToMac(std::string const & address)
{
	unsigned char a[6];
	int last = -1;
	int rc = sscanf(address.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx%n",
		a + 0, a + 1, a + 2, a + 3, a + 4, a + 5,
		&last);
	if (rc != 6 || address.size() != last)
		throw std::runtime_error("invalid mac address format " + address);
	return
		uint64_t(a[0]) << 40 |
		uint64_t(a[1]) << 32 |
		uint64_t(a[2]) << 24 |
		uint64_t(a[3]) << 16 |
		uint64_t(a[4]) << 8 |
		uint64_t(a[5]);
}


Util::Util() {}
