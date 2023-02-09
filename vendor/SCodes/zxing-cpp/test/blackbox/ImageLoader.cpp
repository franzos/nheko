/*
* Copyright 2016 Nu-book Inc.
* Copyright 2019 Axel Waggersauser.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "ImageLoader.h"

#include "BinaryBitmap.h"
#include "ReadBarcode.h"

#include <map>
#include <memory>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace ZXing::Test {

class STBImage : public ImageView
{
	std::unique_ptr<stbi_uc[], void (*)(void*)> _memory;

public:
	STBImage() : ImageView(nullptr, 0, 0, ImageFormat::None), _memory(nullptr, stbi_image_free) {}

	void load(const fs::path& imgPath)
	{
		int width, height, colors;
		_memory.reset(stbi_load(imgPath.string().c_str(), &width, &height, &colors, 1));
		if (_memory == nullptr)
			throw std::runtime_error("Failed to read image");
		ImageView::operator=({_memory.get(), width, height, ImageFormat::Lum});
	}

	operator bool() const { return _data; }
};

std::map<fs::path, STBImage> cache;

void ImageLoader::clearCache()
{
	cache.clear();
}

const ImageView& ImageLoader::load(const fs::path& imgPath)
{
	thread_local std::unique_ptr<BinaryBitmap> localAverage, threshold;

	auto& binImg = cache[imgPath];
	if (!binImg)
		binImg.load(imgPath);

	return binImg;
}

} // namespace ZXing::Test
