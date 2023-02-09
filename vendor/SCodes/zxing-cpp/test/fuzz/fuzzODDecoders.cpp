#include <stdint.h>
#include <stddef.h>

#include "oned/ODMultiUPCEANReader.h"
#include "oned/ODCode39Reader.h"
#include "oned/ODCode93Reader.h"
#include "oned/ODCode128Reader.h"
#include "oned/ODDataBarReader.h"
#include "oned/ODDataBarExpandedReader.h"
#include "oned/ODITFReader.h"
#include "oned/ODCodabarReader.h"
#include "Result.h"

using namespace ZXing;
using namespace ZXing::OneD;

static std::vector<std::unique_ptr<RowReader>> readers;

bool init()
{
	DecodeHints hints;
	readers.emplace_back(new MultiUPCEANReader(hints));
	readers.emplace_back(new Code39Reader(hints));
	readers.emplace_back(new Code93Reader());
	readers.emplace_back(new Code128Reader(hints));
	readers.emplace_back(new ITFReader(hints));
	readers.emplace_back(new CodabarReader(hints));
	readers.emplace_back(new DataBarReader(hints));
	readers.emplace_back(new DataBarExpandedReader(hints));
	return true;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
	if (size < 1)
		return 0;

	static bool inited [[maybe_unused]] = init();
	std::vector<std::unique_ptr<RowReader::DecodingState>> decodingState(readers.size());

	PatternRow row(size * 2 + 1);
	for (size_t i = 0; i < size; ++i){
		auto v = data[i];
		row[i * 2 + 0] = (v & 0xf) + 1;
		row[i * 2 + 1] = (v >> 4) + 1;
	}
	row.back() = 0;

	for (size_t r = 0; r < readers.size(); ++r)
		readers[r]->decodePattern(0, row, decodingState[r]);

	return 0;
}
