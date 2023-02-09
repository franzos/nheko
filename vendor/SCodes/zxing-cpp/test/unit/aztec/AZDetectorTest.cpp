/*
* Copyright 2017 Huy Cuong Nguyen
* Copyright 2013 ZXing authors
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

#include "aztec/AZDetector.h"
#include "BitMatrixIO.h"
#include "PseudoRandom.h"
#include "aztec/AZDetectorResult.h"

#include "gtest/gtest.h"
#include <string_view>
#include <vector>

using namespace ZXing;

namespace {

	struct Point {
		int x;
		int y;
	};

	std::vector<Point> GetOrientationPoints(const BitMatrix& matrix, bool isCompact) {
		int center = matrix.width() / 2;
		int offset = isCompact ? 5 : 7;
		std::vector<Point> result;
		result.reserve(12);
		for (int xSign : { -1, 1}) {
			for (int ySign : { -1, 1}) {
				result.push_back({ center + xSign * offset, center + ySign * offset });
				result.push_back({ center + xSign * (offset - 1), center + ySign * offset });
				result.push_back({ center + xSign * offset, center + ySign * (offset - 1) });
			}
		}
		return result;
	}

	// Zooms a bit matrix so that each bit is factor x factor
	BitMatrix MakeLarger(const BitMatrix& input, int factor) {
		return Inflate(input.copy(), factor * input.width(), factor * input.height(), 0);
	}

	// Test that we can tolerate errors in the parameter locator bits
	void TestErrorInParameterLocator(std::string_view data, int nbLayers, bool isCompact, const BitMatrix &matrix_)
	{
		PseudoRandom random(std::hash<std::string_view>()(data));
		auto orientationPoints = GetOrientationPoints(matrix_, isCompact);
		for (bool isMirror : { false, true }) {
			BitMatrix matrix = matrix_.copy();
			for (int i = 0; i < 4; ++i) {
				// Systematically try every possible 1- and 2-bit error.
				for (int error1 = 0; error1 < Size(orientationPoints); error1++) {
					for (int error2 = error1; error2 < Size(orientationPoints); error2++) {
						BitMatrix copy = matrix.copy();
						if (isMirror) {
							copy.mirror();
						}
						copy.flip(orientationPoints[error1].x, orientationPoints[error1].y);
						if (error2 > error1) {
							// if error2 == error1, we only test a single error
							copy.flip(orientationPoints[error2].x, orientationPoints[error2].y);
						}
						// The detector doesn't seem to work when matrix bits are only 1x1.  So magnify.
						Aztec::DetectorResult r = Aztec::Detector::Detect(MakeLarger(copy, 3), isMirror, true);
						EXPECT_EQ(r.isValid(), true);
						EXPECT_EQ(r.nbLayers(), nbLayers);
						EXPECT_EQ(r.isCompact(), isCompact);
						//DecoderResult res = new Decoder().decode(r);
						//assertEquals(data, res.getText());
					}
				}
				// Try a few random three-bit errors;
				for (int i = 0; i < 5; i++) {
					BitMatrix copy = matrix.copy();
					std::set<size_t> errors;
					while (errors.size() < 3) {
						errors.insert(random.next(size_t(0), orientationPoints.size() - 1));
					}
					for (auto error : errors) {
						copy.flip(orientationPoints[error].x, orientationPoints[error].y);
					}
					Aztec::DetectorResult r = Aztec::Detector::Detect(MakeLarger(copy, 3), false, true);
					EXPECT_EQ(r.isValid(), false);
				}

				matrix.rotate90();
			}
		}
	}
} // anonymous

TEST(AZDetectorTest, ErrorInParameterLocatorZeroZero)
{
	// Layers=1, CodeWords=1.  So the parameter info and its Reed-Solomon info
	// will be completely zero!
	TestErrorInParameterLocator("X", 1, true, ParseBitMatrix(
		"    X X X X X X X   X X X X X \n"
		"X X X X   X     X X         X \n"
		"    X X                 X   X \n"
		"X X X X X X X X X X X X X   X \n"
		"X X   X               X     X \n"
		"X X   X   X X X X X   X     X \n"
		"X X   X   X       X   X   X X \n"
		"      X   X   X   X   X     X \n"
		"X X   X   X       X   X   X X \n"
		"      X   X X X X X   X     X \n"
		"X     X               X     X \n"
		"  X   X X X X X X X X X X X   \n"
		"  X                         X \n"
		"X     X X X X   X     X       \n"
		"X   X     X X X X       X     \n"
		, 'X', true)
	);
}

TEST(AZDetectorTest, ErrorInParameterLocatorCompact)
{
	TestErrorInParameterLocator("This is an example Aztec symbol for Wikipedia.", 3, true, ParseBitMatrix(
		"X     X X       X     X X     X     X         \n"
		"X         X     X X     X   X X   X X       X \n"
		"X X   X X X X X   X X X                 X     \n"
		"X X                 X X   X       X X X X X X \n"
		"    X X X   X   X     X X X X         X X     \n"
		"  X X X   X X X X   X     X   X     X X   X   \n"
		"        X X X X X     X X X X   X   X     X   \n"
		"X       X   X X X X X X X X X X X     X   X X \n"
		"X   X     X X X               X X X X   X X   \n"
		"X     X X   X X   X X X X X   X X   X   X X X \n"
		"X   X         X   X       X   X X X X       X \n"
		"X       X     X   X   X   X   X   X X   X     \n"
		"      X   X X X   X       X   X     X X X     \n"
		"    X X X X X X   X X X X X   X X X X X X   X \n"
		"  X X   X   X X               X X X   X X X X \n"
		"  X   X       X X X X X X X X X X X X   X X   \n"
		"  X X   X       X X X   X X X       X X       \n"
		"  X               X   X X     X     X X X     \n"
		"  X   X X X   X X   X   X X X X   X   X X X X \n"
		"    X   X   X X X   X   X   X X X X     X     \n"
		"        X               X                 X   \n"
		"        X X     X   X X   X   X   X       X X \n"
		"  X   X   X X       X   X         X X X     X \n"
		, 'X', true)
	);
}

TEST(AZDetectorTest, ErrorInParameterLocatorNotCompact)
{
	std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYabcdefghijklmnopqrstuvwxyz";
	TestErrorInParameterLocator(alphabet + alphabet + alphabet, 6, false, ParseBitMatrix(
		"    X   X     X     X     X   X X X X   X   X   X     X X     X X       X X X X   \n"
		"  X         X   X         X X X X X   X   X X X   X   X X X X X   X X X       X   \n"
		"    X   X       X X X X X   X X X X   X X   X X X X X   X X X     X   X X X   X   \n"
		"      X     X     X   X X X X     X   X       X X     X X       X X X         X   \n"
		"X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X \n"
		"X X X               X X X       X           X X X   X     X   X   X X     X X   X \n"
		"        X X X X X X     X   X X   X   X X     X X   X X X X     X X     X     X   \n"
		"X   X X       X   X X X X     X X X X     X X X X   X X X X X       X       X     \n"
		"    X   X X   X X       X     X     X   X   X     X X   X     X X   X   X     X   \n"
		"  X X           X X   X   X       X X       X X X X     X     X X   X             \n"
		"  X     X   X   X X X     X X         X X   X X X X     X X X X X     X X X X   X \n"
		"      X     X X X X X X X X X X   X       X   X X   X     X   X           X X X X \n"
		"X X     X     X X     X   X   X     X   X X X X X X       X X   X       X X   X X \n"
		"    X     X X       X X X X X     X   X           X   X         X   X       X     \n"
		"  X X   X       X         X X X X X X X X X X X X X X X X     X     X X X X X X X \n"
		"X X X       X X   X X X X   X                       X X X   X     X X       X X   \n"
		"  X   X X X X   X   X X   X X   X X X X X X X X X   X         X   X     X   X X   \n"
		"      X     X X X           X   X               X   X     X       X X X   X   X X \n"
		"    X   X       X X     X   X   X   X X X X X   X   X   X X X X   X     X         \n"
		"X   X X         X X X X   X X   X   X       X   X   X X X X   X X X X     X X   X \n"
		"X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X \n"
		"  X       X   X   X X   X   X   X   X       X   X   X X   X X   X X X       X X   \n"
		"  X   X X   X X X X     X X X   X   X X X X X   X   X   X   X   X X     X X   X X \n"
		"  X X       X X X         X X   X               X   X X     X   X X   X   X     X \n"
		"    X   X   X   X X X     X X   X X X X X X X X X   X   X X X X X X     X   X     \n"
		"X   X X           X     X   X                       X   X X   X   X X X     X X   \n"
		"X X X   X X   X     X   X   X X X X X X X X X X X X X X   X   X X X     X   X X   \n"
		"  X   X   X X X               X   X   X     X     X     X   X   X             X   \n"
		"X   X X X   X X     X X       X   X X X X   X X X X X   X X X X X   X   X X     X \n"
		"    X X   X         X X X     X           X       X X   X         X               \n"
		"X X     X     X X     X X     X         X     X X X       X   X X       X   X     \n"
		"  X       X X   X X X     X     X X       X X   X X X     X X       X X     X X   \n"
		"  X X   X   X X X X X       X X       X X X   X X X X   X X X   X X X   X X X X X \n"
		"X X         X X X X   X   X         X X   X X   X     X           X X         X   \n"
		"    X X X X   X X     X   X   X X   X   X   X X X   X X X X X   X   X X X   X     \n"
		"X X       X   X X X         X       X X   X       X X     X X     X X     X   X X \n"
		"X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X \n"
		"X X       X X X       X X     X X     X     X     X           X   X         X     \n"
		"X   X X X   X     X X X   X X X X X   X X   X X X X X     X     X       X   X X   \n"
		"  X   X     X X   X     X X X   X X X X   X   X   X X X X X     X     X       X   \n"
		"        X X       X X X       X X     X X X     X   X     X           X X   X     \n"
		, 'X', true)
	);
}

TEST(AZDetectorTest, ReaderInitFull2Layers)
{
	{
		// Null (not set)
		auto r = Aztec::Detector::Detect(ParseBitMatrix(
			"      X X X   X   X X   X X X X   X X X X X   \n"
			"    X   X X X X   X X X     X X X         X   \n"
			"  X X   X X   X   X X   X X X   X X X     X X \n"
			"  X X X   X X X   X X X X   X   X   X   X   X \n"
			"    X X X X         X               X X X   X \n"
			"X       X X X X X X X X X X X X X X X     X X \n"
			"X     X X X                       X     X   X \n"
			"X   X X X X   X X X X X X X X X   X           \n"
			"X X X   X X   X               X   X       X X \n"
			"X   X X   X   X   X X X X X   X   X   X X   X \n"
			"X   X     X   X   X       X   X   X X X   X X \n"
			"  X   X   X   X   X   X   X   X   X   X   X   \n"
			"  X X X X X   X   X       X   X   X         X \n"
			"X   X   X X   X   X X X X X   X   X X     X   \n"
			"X       X X   X               X   X X X X   X \n"
			"  X   X   X   X X X X X X X X X   X   X X   X \n"
			"X     X X X                       X X         \n"
			"X X   X   X X X X X X X X X X X X X X X X   X \n"
			"X X         X   X       X   X X       X X     \n"
			"    X     X         X X X X   X X X     X X   \n"
			"    X     X X X         X   X X X     X       \n"
			"    X X X         X   X   X X   X       X   X \n"
			"  X   X X X             X   X   X       X     \n"
		), false /*isMirror*/, true /*isPure*/);
		EXPECT_TRUE(r.isValid());
		EXPECT_FALSE(r.readerInit());
		EXPECT_FALSE(r.isCompact());
		EXPECT_EQ(r.nbLayers(), 2);
	}
	{
		// Set
		auto r = Aztec::Detector::Detect(ParseBitMatrix(
			"      X X X   X   X X   X X X X   X X X X X   \n"
			"    X   X X X X   X X X     X X X         X   \n"
			"  X X   X X   X   X X   X X X   X X X     X X \n"
			"  X X X   X X X   X X X X   X   X   X   X   X \n"
			"    X X X X         X   X           X X X   X \n"
			"X       X X X X X X X X X X X X X X X     X X \n"
			"X     X X X                       X     X   X \n"
			"X   X X   X   X X X X X X X X X   X           \n"
			"X X X     X   X               X   X       X X \n"
			"X   X X   X   X   X X X X X   X   X   X X   X \n"
			"X   X   X X   X   X       X   X   X X X   X X \n"
			"  X   X   X   X   X   X   X   X   X   X   X   \n"
			"  X X X X X   X   X       X   X   X         X \n"
			"X   X     X   X   X X X X X   X   X       X   \n"
			"X         X   X               X   X X X X   X \n"
			"  X   X X X   X X X X X X X X X   X   X X   X \n"
			"X     X   X                       X X         \n"
			"X X   X   X X X X X X X X X X X X X X X X   X \n"
			"X X           X X X X         X X     X X     \n"
			"    X     X         X X X X   X X X     X X   \n"
			"    X     X X X         X   X X X     X       \n"
			"    X X X         X   X   X X   X       X   X \n"
			"  X   X X X             X   X   X       X     \n"
		), false /*isMirror*/, true /*isPure*/);
		EXPECT_TRUE(r.isValid());
		EXPECT_TRUE(r.readerInit());
		EXPECT_FALSE(r.isCompact());
		EXPECT_EQ(r.nbLayers(), 2);
	}
}

TEST(AZDetectorTest, ReaderInitFull22Layers)
{
	// Set
	auto r = Aztec::Detector::Detect(ParseBitMatrix(
		"            X X   X   X X X     X       X   X     X         X   X     X X   X   X       X X   X X X         X       X   X     X   X         X X X X X X   X   X   X   X X   X X     X     X X X X     X   X X X X   X X   \n"
		"        X X     X X   X   X X X X   X X   X           X     X   X   X X   X   X X X X X X X       X   X         X X X   X X     X X   X             X         X   X X   X X   X   X   X   X X     X X X X     X     X   X \n"
		"    X   X   X   X       X           X X     X       X       X   X X       X X X     X   X   X X     X X   X X X X         X X X   X     X X X   X   X X     X       X X X   X   X     X X   X X X           X X   X X     \n"
		"      X X       X X     X X X     X       X     X   X   X       X     X       X             X   X     X X         X   X   X X   X       X     X X   X           X X X X   X   X X     X     X     X X         X   X   X   \n"
		"        X   X       X X   X       X X X X   X X X X X X X X X   X X X   X X X   X   X X X X X X X   X X X X X     X       X   X     X       X X X     X         X X X   X X X X X           X   X X X X X X X X       X   \n"
		"    X     X   X     X   X X         X X X X     X     X X X   X X     X       X X   X X     X X X X X X X X   X   X   X     X     X X   X           X           X     X         X   X   X X   X     X X X X   X X         \n"
		"X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X \n"
		"          X     X X     X X X X       X   X     X       X X           X X X               X   X     X X X X   X X X   X         X X X         X X   X X X   X   X X                     X         X X X   X         X X X \n"
		"X   X X X X X         X X X   X   X X       X X   X X X   X X X             X   X X       X   X       X     X X       X   X   X X   X X X X X   X   X   X       X X         X X     X X X     X X X X       X X       X   \n"
		"  X X         X X X     X X X X   X X X X X   X         X       X   X X   X     X X X X X X   X X   X X   X   X       X   X   X X X     X X   X     X   X X   X X X X X   X     X     X X     X X     X X X       X   X   \n"
		"  X   X     X X           X X   X X         X   X   X X X   X   X X X X X X X X X       X   X   X X X X     X     X   X X X X X             X X X X X X     X     X     X   X   X     X           X     X X X   X X X   X \n"
		"  X X X X X   X X     X   X     X X X X   X         X X X X X X               X X X   X     X X X X     X X   X X           X X   X X X   X   X X   X X           X X X X X       X     X         X X X X X   X     X X X \n"
		"  X     X X X X       X X     X       X   X X X X X X X X   X X X   X X X X X     X X   X     X   X   X X   X X             X X X X   X X   X X   X X   X     X     X       X X         X X X X X X X       X X       X   \n"
		"X X X             X X   X X   X X X   X   X     X   X   X X X X   X   X X         X         X       X     X     X       X X         X         X   X X   X     X X       X     X   X X X X   X X     X X X X   X   X X X   \n"
		"X X   X   X X     X   X X X     X X   X     X   X X X   X X X     X         X X     X X X X X X X X         X X     X X X X       X X X     X X X X X X   X X     X     X X X X         X X X   X   X   X X X       X   X \n"
		"  X X X   X     X X   X X         X X X X X   X X       X   X   X X       X       X X X     X X     X X X           X X   X X X X             X X X     X X   X X X       X   X X     X             X X   X     X X   X X \n"
		"X X     X   X     X     X X X X X X     X X X     X X X   X   X X X     X X X   X X X X X   X       X X X   X X     X X X   X   X X   X X   X   X X X     X   X X X       X X X     X           X   X X     X     X X     \n"
		"      X         X     X X   X         X X X         X   X             X   X   X X   X       X   X   X   X                 X   X   X X   X     X X       X X X X X       X       X X         X   X X             X   X X   \n"
		"  X X   X   X   X X     X       X   X X X X X   X X   X       X X           X X   X X   X X     X X X X     X X   X     X X     X   X X     X X   X X   X   X X X X     X X X X X       X X X     X     X X X     X       \n"
		"X X X X       X X X   X       X     X X   X     X X           X   X X X   X         X         X       X       X     X X     X X     X   X X     X X       X   X X       X     X         X   X     X X X X X     X       X \n"
		"    X   X   X X X X X     X X X X   X   X   X X       X   X   X         X   X X   X     X X       X X       X   X     X           X   X     X     X X   X       X   X X     X X X X   X X X       X X X     X X           \n"
		"  X   X       X X   X X X       X X X   X     X X   X       X             X       X     X X X X X   X X       X X         X X   X X   X X             X     X   X X   X X     X   X     X       X X X X X     X   X X   X \n"
		"X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X \n"
		"      X X     X X   X X X X X     X X     X   X     X X X X     X   X X       X   X X X       X X     X X X     X   X X         X     X X     X       X       X X X   X         X   X   X X     X   X   X X   X   X   X X \n"
		"          X X     X X       X X     X     X X       X     X X X X X   X     X         X   X X X   X         X   X   X     X X   X X X       X   X X   X       X X   X   X X X X X   X X   X X X   X     X   X   X X X     \n"
		"X   X X X           X X X X   X     X   X X   X   X           X X X   X X     X X   X         X X         X   X         X   X X X     X X     X   X   X X       X X X X       X             X           X             X   \n"
		"X X   X     X   X   X X X X X     X X     X X   X X X   X   X   X   X       X     X   X X X X X X       X   X X         X   X     X X       X   X X   X         X   X     X X X X     X   X X           X X X         X X \n"
		"  X       X           X X X X X X   X X X     X   X   X   X X   X     X   X         X X   X   X                 X X   X X   X X     X         X X X     X X     X     X   X   X   X X X     X   X X X X                 X \n"
		"  X     X X X     X X X     X     X   X   X X X X X   X   X           X   X X   X       X X   X   X   X X X X X   X X       X X X   X   X   X X   X X X X X X X X   X X X   X X               X   X     X   X       X X   \n"
		"  X     X     X X     X X X           X X X   X X   X X   X X X   X   X       X X X X     X         X X   X   X           X X X X X   X   X       X X X       X X         X   X X X X     X       X X   X X   X   X X X X \n"
		"    X X   X X   X               X X   X X X X X X X           X   X   X X   X X     X     X X         X   X X X X X   X X     X   X   X X   X X X   X X X X   X     X X X   X X     X   X   X   X X     X   X   X   X X X \n"
		"X   X X   X   X         X     X X   X X         X X     X         X   X   X     X     X X X   X   X     X     X         X   X X X     X   X   X X     X X X X X X   X         X X       X   X   X     X   X     X     X X \n"
		"  X         X X   X     X   X     X X   X   X   X   X X   X     X X X X   X X     X   X X X X     X X       X   X   X X X X X X       X   X X     X   X     X X X X X X   X X X X       X   X X   X   X   X X X     X   X \n"
		"X                 X   X   X X   X       X X               X X   X X   X   X   X   X X   X X X   X     X   X   X X     X X     X   X     X X   X X     X   X X X X     X X     X X     X X       X X X         X   X X     \n"
		"X         X X X X   X X X X     X   X   X   X   X       X   X X         X X X X         X   X X X X X       X X X X X X X     X X     X   X X     X         X   X X   X X X X X X X X X   X X X   X     X   X   X X X X   \n"
		"  X   X   X       X     X X X X   X X X X     X   X X X X X   X   X X X   X     X   X   X     X         X X   X X X   X X         X X X X X     X   X       X     X       X   X X         X   X X X     X         X X   X \n"
		"X   X     X X     X       X   X   X         X X X X   X X     X   X   X   X X           X         X   X     X               X X     X       X       X X X X   X X X X X     X X   X   X     X     X X X     X     X X   X \n"
		"  X X   X     X                 X             X X X     X       X   X X X X   X   X     X X X   X X X X   X     X   X   X   X X X   X X X     X X X X   X X       X     X       X             X   X X X X X     X X X     \n"
		"X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X \n"
		"      X   X     X X   X X       X X   X   X             X   X X X     X   X     X X   X   X   X   X     X X             X X X       X X   X   X   X     X     X X X   X X           X   X X   X     X   X X     X X X   X \n"
		"X X X   X   X X     X     X   X X       X   X X     X     X     X           X X         X   X   X X X     X X   X   X X X     X X   X       X X X X   X     X X             X X   X X X     X     X X X     X X   X     X \n"
		"X     X   X       X X               X X   X           X   X       X     X X   X           X   X   X   X       X             X   X X   X         X X X X X     X X X X                     X   X X X X X       X X X X     \n"
		"    X   X X X     X X X X X X X X X X X X X X X X   X   X     X X           X     X   X X   X X   X         X     X   X X X     X   X X     X X X   X   X X   X   X         X   X X X   X X X X X X     X   X X     X X   \n"
		"  X X   X     X   X X       X X               X           X X   X X X X X     X         X     X X   X X X X         X     X     X     X   X   X   X X   X   X   X     X   X     X   X X X X X     X     X X   X X X X X X \n"
		"X X X   X X X X   X X X   X X X X X     X X X       X           X           X X             X X X   X X   X X X X   X X X X X   X X     X X X X X X     X                   X X     X   X X X   X X     X   X X X         \n"
		"X X X   X     X   X X     X     X       X     X           X       X X   X       X X   X X     X X     X X X   X X X   X X       X X X     X   X   X   X X   X X X     X X     X X X X   X   X X X   X X X X   X X   X   X \n"
		"  X   X X X X   X   X       X X   X         X       X     X   X             X X   X         X X         X X X   X         X X   X   X X X   X   X   X X       X X     X   X X X X X                   X   X X X X X     X \n"
		"X         X   X   X X X X X   X X   X   X X   X   X X   X X X X   X   X           X     X     X X X   X   X   X           X X X         X X   X X X X X     X X     X     X   X     X X       X   X     X X   X X X X   X \n"
		"      X X X X X X       X   X     X X X     X   X   X           X X X   X X X X X X X   X X   X X X X X X X X X X X X X X X X         X X X X       X X   X X       X       X       X   X X       X X     X X     X X X   \n"
		"X   X   X X   X     X X X X   X     X     X       X     X   X   X X                   X     X   X                       X       X X     X       X                 X X   X         X X         X       X X           X     \n"
		"            X X   X X X               X X X X   X       X X   X   X   X     X X X X   X X X     X   X X X X X X X X X   X     X X X   X     X   X   X   X X   X   X     X X X     X   X   X X X     X   X X X X X   X   X \n"
		"X X     X X         X   X     X X X X     X   X         X   X   X   X X X X   X X X X   X X   X X   X               X   X     X     X X X     X             X X   X X     X   X X   X   X X   X         X X   X X         \n"
		"X X   X X X X X       X X X X X     X X   X X   X X X   X       X         X X X X   X   X X     X   X   X X X X X   X   X         X X   X X X   X X   X X     X     X X X   X   X   X X         X X X   X X X X X X       \n"
		"X X   X X X   X     X   X     X X X X   X X       X   X X   X X   X           X   X X X     X   X   X   X       X   X   X     X   X X     X   X X   X       X   X   X     X     X   X X X   X   X X   X   X   X X X X   X \n"
		"X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X \n"
		"X             X X X X   X   X             X           X X   X         X       X X X X   X X   X X   X   X       X   X   X X       X X   X     X X X   X X   X X X X   X X X       X     X     X         X X         X X   \n"
		"X     X     X   X     X X X     X X     X X X X   X X X   X X     X     X X X X   X X     X     X   X   X X X X X   X   X X   X X     X   X X   X     X           X X   X   X     X X     X         X       X     X   X   \n"
		"X X   X X           X   X X X X       X X X   X   X   X     X   X X     X     X   X   X X   X   X   X               X   X   X   X X     X X   X X X   X   X         X     X     X X       X             X X   X   X       \n"
		"X     X   X X X     X   X X   X   X     X   X X       X       X X     X X X X   X               X   X X X X X X X X X   X       X   X       X X       X     X     X     X X X         X X   X   X     X     X X X X X X X \n"
		"  X             X   X               X   X X   X     X X X X X X X             X       X X   X X X                       X   X X   X   X       X X X X X X         X X           X   X X         X   X   X     X X   X X   \n"
		"X       X X X X   X       X   X X X X       X   X X X   X   X X X   X   X   X   X X       X     X X X X X X X X X X X X X X           X X   X X     X     X X X X   X       X     X X X X X     X X X     X X X       X   \n"
		"X   X X X     X   X X X X X   X X     X   X         X   X   X         X X X     X X X   X X         X         X   X X       X X   X     X     X       X         X X X X X     X   X X X X X   X   X       X   X X X X   X \n"
		"X X X X X   X         X     X     X X   X X X X   X     X X X   X X   X X   X   X   X X X X         X X     X   X X                   X   X X     X   X   X X   X X         X   X     X X         X X     X X   X X   X X \n"
		"      X X X   X     X X X X X   X X   X X     X   X X X X         X X   X     X X X   X X       X   X X X X     X X X X   X X   X             X X X         X X   X X X   X     X   X   X X     X X       X       X       \n"
		"    X X     X X X           X X     X X   X X X X X X X X X X X   X       X X X X     X X   X X X   X   X X X       X X     X X X X X X X   X X   X     X X         X X     X X X   X   X   X X   X   X   X X   X   X     \n"
		"  X X   X X   X X       X     X X       X     X       X X X   X X X           X X   X X             X   X X         X   X X X   X X   X X X   X X X       X X X     X X X X   X X X   X X X     X         X     X       X \n"
		"  X X X     X     X         X X   X   X X X X X   X   X     X       X   X   X     X               X   X X   X X   X X     X X X           X X     X X         X       X     X     X   X X   X X           X X   X X X     \n"
		"X   X   X     X X     X   X X   X   X X X         X       X X                       X   X       X X X   X       X       X   X     X   X   X     X     X X   X   X   X X X X   X X X X X     X X X X X X X     X X   X X   \n"
		"X X   X X   X       X X     X     X X X   X X   X X X X     X X   X X X     X   X     X   X X X   X   X   X X   X   X X X X     X X   X     X X     X X   X       X         X       X   X   X X     X   X   X         X X \n"
		"X X X   X       X X   X     X X   X   X   X   X X X   X   X   X   X   X X                                 X   X     X     X     X   X X   X   X     X   X X         X   X X     X X   X X   X X X X           X   X       \n"
		"X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X \n"
		"X   X   X       X   X   X   X   X   X   X X       X X       X     X   X   X     X X     X     X X X X   X X       X   X     X   X X X     X   X X X X   X X   X X         X       X X X X X X X     X     X   X   X     X \n"
		"X X X X     X X X X   X       X X   X X   X X X X   X   X       X X   X     X X   X X     X X   X X X     X X   X X X   X   X X   X X   X X X X X X   X X X X X X X X   X X X   X X X       X   X   X     X X X X X X     \n"
		"  X     X         X X X       X   X     X     X X X   X X   X   X X           X   X X X   X X     X     X X   X X X   X X   X X       X X       X   X X   X   X     X     X     X   X       X     X X     X   X         X \n"
		"X   X   X X X       X   X     X     X X X X X   X   X   X     X   X   X   X X X   X       X X   X X X X X   X       X         X   X X X X X X X X X X           X   X   X X X   X X   X X             X   X X X X X     X \n"
		"    X X X     X X   X X   X X X       X   X     X       X     X X                     X   X X   X       X X       X X   X X X X X   X   X     X     X   X     X   X   X X     X X X X   X     X X       X X     X X     X \n"
		"X X X   X   X     X       X       X X X X X X X X       X   X     X     X X X X       X   X X   X   X X     X X X   X   X   X     X     X X X   X   X X X         X         X       X X X     X     X X X X X   X X X X   \n"
		"X X     X X         X   X X X X X   X   X       X     X           X           X X   X   X   X X   X   X X     X X X       X X X X X   X   X   X     X     X X X   X   X X           X X X   X X X       X         X X     \n"
		"X       X X X X X X X     X     X     X X   X   X X X   X X       X     X X X X   X         X X   X X   X   X X X   X X   X   X X   X   X X X X           X   X     X X X   X       X     X           X X   X X X         \n"
		"    X X X                 X X     X   X X X   X   X X X   X       X X     X         X   X X X X       X       X X X X X     X X   X X X   X   X X X X   X X       X   X X X     X X X   X       X X     X X   X     X X   \n"
		"  X X X   X X X X X X X X   X       X X     X                 X   X       X X X       X   X X   X X X X   X X   X   X             X         X X X   X X       X X X X       X X   X X   X X X     X X X     X X X     X   \n"
		"X       X X   X X X X   X X   X     X X   X           X X   X X   X X X   X   X X       X X X   X X X X   X   X     X   X   X X   X     X X     X     X X   X   X X     X X     X       X X   X               X           \n"
		"X           X   X X X X X X   X     X     X X X       X X   X     X X     X X X   X   X X   X X X X X X   X X X X       X X X   X X         X     X X X X       X X X   X X X   X X X X   X   X     X X     X X X   X     \n"
		"  X           X X     X   X X X       X       X X X     X X X X X X X     X           X     X   X   X   X     X   X   X X X   X         X X   X     X X X X X X   X X X         X X X X X     X       X X X       X     X \n"
		"    X   X   X   X     X X X X X   X X X X   X X X X X X   X   X X   X X X X X X X X   X   X   X   X   X X   X X     X X     X     X   X X X X X   X X X   X X X X         X X X   X X   X X X X   X   X     X X       X   \n"
		"X X   X X     X   X     X X       X   X X X   X X X X     X     X   X     X       X   X   X X   X       X X     X   X X   X X X X X X X             X X X X     X X X X X X       X X   X X     X     X X X           X X \n"
		"X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X \n"
		"    X X X           X   X X X       X X X     X     X X   X X   X X X         X     X     X X X X X   X X X   X X X   X       X       X X X     X     X       X     X X       X                       X X           X     \n"
		"X X X   X X X     X   X X   X X X X   X     X X X X   X X X   X   X X X X   X     X X     X   X   X X X     X X   X X       X X     X     X X X   X X   X X X X   X       X X   X X   X X X X X       X X   X   X X       \n"
		"    X     X   X     X   X X X X       X           X       X X X X   X X   X     X X         X X X             X X X X           X X X   X X             X X X   X   X     X   X X X   X X     X X X     X X   X X X X X   \n"
		"      X X X X X   X X     X X X X     X X   X X     X           X           X X     X X X X X       X     X X X X   X       X   X   X X X X X   X   X X X   X   X X X X   X X   X X X     X   X   X       X X X X   X X X \n"
		"X X X   X         X X     X     X       X     X   X         X X X X   X   X     X         X       X   X X X       X X X X X   X X   X X   X       X X       X X X   X X       X X   X X       X X X       X           X X \n"
		"X X     X X X   X                 X X X     X X X   X X X X X   X X X X   X X   X   X X X   X X   X   X   X X   X     X X X   X         X   X X       X     X X       X     X X       X     X X X X   X     X X   X X   X \n"
		"  X X   X     X   X   X X   X   X   X           X     X       X   X   X   X   X X X   X X   X X   X X X   X   X   X X X X X       X X X       X     X   X   X X X     X       X X X X   X X X   X   X   X X     X X X   X \n"
		"X X   X X X X X X   X   X X   X   X         X X X X   X       X X           X   X X     X     X X     X X X X         X     X X   X     X   X X X X       X         X X   X X X X X     X X   X X     X     X X X     X X \n"
		"X X     X X   X X   X X X X   X       X   X     X   X X         X   X               X     X X X   X X X X X       X   X X   X X     X   X         X X X   X   X X   X   X     X X X X X X     X X X X X X X   X X       X \n"
		"    X   X X X X   X   X   X           X X X X   X   X     X X   X   X X X   X X X     X X X X X     X X X   X X X X X       X X   X X       X X X X X             X   X X   X X X           X X X         X X X   X   X X \n"
		"X     X   X   X   X   X X   X   X   X         X X X         X   X   X X       X         X     X X X   X   X   X   X X X X X X         X X     X X   X X   X   X       X X       X X     X X X       X X       X   X     X \n"
		"X X X X     X X X   X X X     X X X     X   X X X     X       X X X   X   X X X X       X X     X X X X X   X         X   X X X X X X X   X X X X X X X   X     X X   X   X X     X X X     X X       X X   X X X     X X \n"
		"  X   X   X             X X X   X       X X   X X   X   X         X   X X X       X     X               X X   X X X X X X         X   X X X     X X X   X   X     X X         X X   X X X   X     X   X X X       X X X X \n"
		"X       X   X X   X X   X X X   X   X X X   X   X X X     X X X X X X   X X X X X     X     X X X X   X X   X       X X X X   X X       X X X   X X X X X X X       X X X X X X X X           X X       X X X         X   \n"
		"X     X X       X X X         X           X     X       X X     X X X     X   X X X   X   X       X     X     X X X X X X X   X   X X X             X   X X X     X X   X X   X X X X X X   X     X X X       X           \n"
		"X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X   X \n"
		"X     X X     X X X   X X         X X X X X       X X       X     X   X   X         X   X   X       X X   X   X     X X X X   X     X           X     X       X X X X     X     X       X X X   X X X X   X   X   X X X   \n"
		"  X X X X   X   X   X X       X   X     X   X   X X X X     X     X         X     X     X   X   X   X     X X   X X     X   X   X           X X                     X X     X   X   X   X   X X   X X     X X X X   X X   \n"
		"X X   X X X   X   X   X X X X       X X         X         X   X X X X     X       X X X X   X   X     X X             X X X X X   X   X       X X X X X     X X X X X X X X     X X X X   X X X X     X X X   X X X   X X \n"
		"X X X     X X   X   X   X X X   X X X     X X X X   X   X X X   X   X X   X X       X             X X X     X   X X   X X     X X           X X X             X X     X   X X       X X X X X X         X   X   X       X \n"
		"  X   X   X   X   X   X X X X     X X X X     X X       X X X   X X X     X     X X X X X X X X       X X     X X   X   X       X         X   X X   X X X X   X   X   X X     X     X X   X X   X X X X   X           X   \n"
		"X X X X X   X X X X   X X X   X X X         X X       X       X       X X X X X X X     X   X X X         X X       X X X X X   X     X   X X X X X   X   X     X           X X X   X X X X X X       X X X X       X X   \n"
	), false /*isMirror*/, true /*isPure*/);
	EXPECT_TRUE(r.isValid());
	EXPECT_TRUE(r.readerInit());
	EXPECT_FALSE(r.isCompact());
	EXPECT_EQ(r.nbLayers(), 22);
}

TEST(AZDetectorTest, ReaderInitCompact)
{
	{
		// Null (not set)
		auto r = Aztec::Detector::Detect(ParseBitMatrix(
			"            X X   X         X \n"
			"    X X X X   X X X   X     X \n"
			"    X X             X   X     \n"
			"  X X X X X X X X X X X X     \n"
			"  X   X               X       \n"
			"    X X   X X X X X   X     X \n"
			"X     X   X       X   X X     \n"
			"X     X   X   X   X   X     X \n"
			"X     X   X       X   X X X   \n"
			"X X   X   X X X X X   X X X X \n"
			"    X X               X   X   \n"
			"      X X X X X X X X X X X   \n"
			"X             X X         X X \n"
			"X         X   X     X X       \n"
			"X X X X     X X X         X X \n"
		), false /*isMirror*/, true /*isPure*/);
		EXPECT_TRUE(r.isValid());
		EXPECT_FALSE(r.readerInit());
		EXPECT_TRUE(r.isCompact());
		EXPECT_EQ(r.nbLayers(), 1);
	}
	{
		// Set
		auto r = Aztec::Detector::Detect(ParseBitMatrix(
			"            X X   X         X \n"
			"    X X X X   X X X   X     X \n"
			"    X X     X       X   X     \n"
			"  X X X X X X X X X X X X     \n"
			"  X X X               X       \n"
			"    X X   X X X X X   X X   X \n"
			"X   X X   X       X   X X     \n"
			"X     X   X   X   X   X     X \n"
			"X     X   X       X   X   X   \n"
			"X X   X   X X X X X   X   X X \n"
			"    X X               X   X   \n"
			"      X X X X X X X X X X X   \n"
			"X       X X   X   X X     X X \n"
			"X         X   X     X X       \n"
			"X X X X     X X X         X X \n"
		), false /*isMirror*/, true /*isPure*/);
		EXPECT_TRUE(r.isValid());
		EXPECT_TRUE(r.readerInit());
		EXPECT_TRUE(r.isCompact());
		EXPECT_EQ(r.nbLayers(), 1);
	}
}
