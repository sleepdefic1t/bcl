/* 
 * A runnable main program that tests the functionality of class Uint256.
 * 
 * Bitcoin cryptography library
 * Copyright (c) Project Nayuki
 * 
 * https://www.nayuki.io/page/bitcoin-cryptography-library
 * https://github.com/nayuki/Bitcoin-Cryptography-Library
 */

#include "gtest/gtest.h"

#include "TestHelper.hpp"
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Uint256.hpp"


using namespace bcl;


/*---- Structures ----*/

struct BinaryCase {
	const char *x;
	const char *y;
};

struct TernaryCase {
	const char *x;
	const char *y;
	const char *z;
	unsigned int bit;  // 0 or 1
};


/*---- Test cases ----*/

TEST(uint256, comparison) {
	const size_t CASE_SIZE = 7U;
	const array<BinaryCase, CASE_SIZE> cases{{  // All hexadecimal strings must be in uppercase for strcmp() to work properly
		{"0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000001"},
		{"0000000000000000000000000000000000000000000000000000000000000080", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"FF00000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"F000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"0123400000000000000000000000000000000000000000000000000000000000", "0123400000000000000000000000000000000000000000000000000000000000"},
		{"0000000000000000000000000000000000000000000000000000000000000000", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F"},
	}};
	for (const BinaryCase &tc : cases) {
		int cmp = std::strcmp(tc.x, tc.y);
		Uint256 x(tc.x);
		Uint256 y(tc.y);
		assert((x == y) == (cmp == 0));
		assert((x != y) == (cmp != 0));
		assert((x <  y) == (cmp <  0));
		assert((x >  y) == (cmp >  0));
		assert((x <= y) == (cmp <= 0));
		assert((x >= y) == (cmp >= 0));
	}
}


TEST(uint256, add) {
	const size_t CASE_SIZE = 7U;
	const array<TernaryCase, CASE_SIZE> cases{{
		{"0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000", 0},
		{"0000000000000000000000000000000000000000000000000000000080000000", "0000000000000000000000000000000000000000000000000000000080000000", "0000000000000000000000000000000000000000000000000000000100000000", 0},
		{"000000000000000000000000000000000000000000000FFFFFFFFFFFFFFFFFFF", "0000000000000000000000000000000000000000000000000000000000000001", "0000000000000000000000000000000000000000000010000000000000000000", 0},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2E", "0000000000000000000000000000000000000000000000000000000000000001", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F", 0},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", "0000000000000000000000000000000000000000000000000000000000000001", "0000000000000000000000000000000000000000000000000000000000000000", 1},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE", 1},
		{"FF00000000000000000000000000000000000000000000000000000000000000", "FF00000000000000000000000000000000000000000000000000000000000000", "FE00000000000000000000000000000000000000000000000000000000000000", 1},
	}};
	for (const TernaryCase &tc : cases) {
		Uint256 x(tc.x);
		Uint256 y(tc.y);
		ASSERT_TRUE(x.add(y, 0) == 0);
		ASSERT_TRUE(x == Uint256(tc.x));
		ASSERT_TRUE(x.add(y, 1) == tc.bit);
		ASSERT_TRUE(x == Uint256(tc.z));
	}
}


TEST(uint256, subtract) {
	const size_t CASE_SIZE = 4U;
	const array<TernaryCase, CASE_SIZE> cases{{
		{"0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000", 0},
		{"0000000000000000000000000000000000000000000000000000000000000003", "0000000000000000000000000000000000000000000000000000000000000002", "0000000000000000000000000000000000000000000000000000000000000001", 0},
		{"0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000001", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", 1},
		{"0000000000000000000000000000000000000000000000000000000000000000", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", "0000000000000000000000000000000000000000000000000000000000000001", 1},
	}};
	for (const TernaryCase &tc : cases) {
		Uint256 x(tc.x);
		Uint256 y(tc.y);
		assert(x.subtract(y, 0) == 0);
		assert(x == Uint256(tc.x));
		assert(x.subtract(y, 1) == tc.bit);
		assert(x == Uint256(tc.z));
	}
}


TEST(uint256, shift_left_1) {
	const size_t CASE_SIZE = 12U;
	const array<TernaryCase, CASE_SIZE> cases{{
		{"0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000", "", 0},
		{"0000000000000000000000000000000000000000000000000000000000000001", "0000000000000000000000000000000000000000000000000000000000000002", "", 0},
		{"0000000000000000000000000000000000000000000000000000000080000000", "0000000000000000000000000000000000000000000000000000000100000000", "", 0},
		{"00000000000000000000000000000000000000000000000000000000FFFFFFFF", "00000000000000000000000000000000000000000000000000000001FFFFFFFE", "", 0},
		{"000ABCDEF0000000000000000000000000000000000000000000000000000000", "001579BDE0000000000000000000000000000000000000000000000000000000", "", 0},
		{"09F4BE4173C99D20A7AC925A3ED6E95A3D2EFA730B4BF7956168F2DEA24BFC27", "13E97C82E7933A414F5924B47DADD2B47A5DF4E61697EF2AC2D1E5BD4497F84E", "", 0},
		{"20E44F4466B9BDF9B599EEF505DF252B7022FFD23A8EBBEB547D12948492345F", "41C89E88CD737BF36B33DDEA0BBE4A56E045FFA4751D77D6A8FA2529092468BE", "", 0},
		{"44BE7E70EF2D3FD4E40EB8F516F9ED6CCFDFFE5E70162528077CBBFE5B24199F", "897CFCE1DE5A7FA9C81D71EA2DF3DAD99FBFFCBCE02C4A500EF977FCB648333E", "", 0},
		{"8288BA5463D3A027F5BCC6A05FD1D4A1B3C0B0E54B9750CF6C622F90A15E5B66", "051174A8C7A7404FEB798D40BFA3A943678161CA972EA19ED8C45F2142BCB6CC", "", 1},
		{"8000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000", "", 1},
		{"FFFF000000000000000000000000000000000000000000000000000000000000", "FFFE000000000000000000000000000000000000000000000000000000000000", "", 1},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE", "", 1},
	}};
	for (const TernaryCase &tc : cases) {
		Uint256 x(tc.x);
		assert(x.shiftLeft1() == tc.bit);
		assert(x == Uint256(tc.y));
	}
}


TEST(uint256, shift_right_1) {
	const size_t CASE_SIZE = 11U;
	const array<BinaryCase, CASE_SIZE> cases{{
		{"0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"0000000000000000000000000000000000000000000000000000000000000001", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"0000000000000000000000000000000000000000000000000000000100000000", "0000000000000000000000000000000000000000000000000000000080000000"},
		{"00000000000000000000000000000000000000000000000000000000FFFFFFFF", "000000000000000000000000000000000000000000000000000000007FFFFFFF"},
		{"0D659F38B34258B59E95248E384EC1C13280C3AE4427F6F86A7BBA35979B789F", "06B2CF9C59A12C5ACF4A92471C2760E0994061D72213FB7C353DDD1ACBCDBC4F"},
		{"58C8E14F273F86A7EE70B95ABF8BD7057C50837FF6F4885A822032D571E62ED8", "2C6470A7939FC353F7385CAD5FC5EB82BE2841BFFB7A442D4110196AB8F3176C"},
		{"D5597ACA79B909E0455B908AEF1F060FE213F6832D7B9BA28F3A2572B0CE3D00", "6AACBD653CDC84F022ADC845778F8307F109FB4196BDCDD1479D12B958671E80"},
		{"DA5DC928FA00D056242652DAD1B25D01D8747E0A32A4C99FCAB44AFF4A2A4B26", "6D2EE4947D00682B1213296D68D92E80EC3A3F05195264CFE55A257FA5152593"},
		{"000ABCDEF0000000000000000000000000000000000000000000000000000000", "00055E6F78000000000000000000000000000000000000000000000000000000"},
		{"FFFF000000000000000000000000000000000000000000000000000000000000", "7FFF800000000000000000000000000000000000000000000000000000000000"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"},
	}};
	for (const BinaryCase &tc : cases) {
		Uint256 x(tc.x);
		x.shiftRight1(0);
		assert(x == Uint256(tc.x));
		x.shiftRight1(1);
		assert(x == Uint256(tc.y));
	}
}


TEST(uint256, reciprocal) {
	#ifdef USE_EMBEDDED
	const size_t CASE_SIZE = 50U;
	#else
	const size_t CASE_SIZE = 99U;
	#endif
	const array<TernaryCase, CASE_SIZE> cases{{
		{"0000000000000000000000000000000000000000000000000000000000000006", "000000000000000000000000000000000000000000000000000000000000000B", "0000000000000000000000000000000000000000000000000000000000000002", 0},
		{"000000000000000000000000000000000000000000000000000000000000000A", "000000000000000000000000000000000000000000000000000000000000000B", "000000000000000000000000000000000000000000000000000000000000000A", 0},
		{"000000000000000000000000000000000000000000000000000000000000000A", "000000000000000000000000000000000000000000000000000000000000000B", "000000000000000000000000000000000000000000000000000000000000000A", 0},
		{"0000000000000000000000000000000000000000000000000000000000226264", "0000000000000000000000000000000000000000000000000000000000BCD627", "00000000000000000000000000000000000000000000000000000000008909A4", 0},
		{"000000000000000000000000000000000000000000000000000000000054B08A", "0000000000000000000000000000000000000000000000000000000000BCD627", "00000000000000000000000000000000000000000000000000000000001D5B53", 0},
		{"000000000000000000000000000000000000000000000000000000000059FC3D", "0000000000000000000000000000000000000000000000000000000005CF4103", "000000000000000000000000000000000000000000000000000000000083034F", 0},
		{"000000000000000000000000000000000000000000000000000000000069E33C", "0000000000000000000000000000000000000000000000000000000000BCD627", "00000000000000000000000000000000000000000000000000000000002CF8D0", 0},
		{"0000000000000000000000000000000000000000000000000000000001C0ABCC", "0000000000000000000000000000000000000000000000000000000005CF4103", "00000000000000000000000000000000000000000000000000000000008C5C37", 0},
		{"0000000000000000000000000000000000000000000000000000000004CD5694", "0000000000000000000000000000000000000000000000000000000005CF4103", "000000000000000000000000000000000000000000000000000000000359D9DA", 0},
		{"0000000000000000000000000000000000000000000000000000003AC01B70A8", "0000000000000000000000000000000000000000000000000000009D4BBED6BB", "000000000000000000000000000000000000000000000000000000698B8CBC44", 0},
		{"0000000000000000000000000000000000000000000000000000006123D35703", "0000000000000000000000000000000000000000000000000000009D4BBED6BB", "0000000000000000000000000000000000000000000000000000001FE6C16A14", 0},
		{"0000000000000000000000000000000000000000000000000000008618728DC5", "0000000000000000000000000000000000000000000000000000009D4BBED6BB", "0000000000000000000000000000000000000000000000000000002EBE4D800F", 0},
		{"0000000000000000000000000000000000000000000000000000182E3D1E491A", "0000000000000000000000000000000000000000000000000000326146180D77", "000000000000000000000000000000000000000000000000000016CCD1557B20", 0},
		{"00000000000000000000000000000000000000000000000000001E49410D720C", "0000000000000000000000000000000000000000000000000000326146180D77", "00000000000000000000000000000000000000000000000000002527E2D7BA3A", 0},
		{"0000000000000000000000000000000000000000000000000000303881C9B0F2", "0000000000000000000000000000000000000000000000000000326146180D77", "0000000000000000000000000000000000000000000000000000138C6F73F28C", 0},
		{"0000000000000000000000000000000000000000000000000001149E23B1F140", "00000000000000000000000000000000000000000000000000018432130E284B", "00000000000000000000000000000000000000000000000000017BDC9A14AB1B", 0},
		{"00000000000000000000000000000000000000000000000000015E05404A0AFF", "00000000000000000000000000000000000000000000000000018432130E284B", "00000000000000000000000000000000000000000000000000002896A661893E", 0},
		{"00000000000000000000000000000000000000000000000000016913D1359D4B", "00000000000000000000000000000000000000000000000000018432130E284B", "000000000000000000000000000000000000000000000000000101915A99C0C3", 0},
		{"0000000000000000000000000000000000000000000000000D1C47F15583F153", "000000000000000000000000000000000000000000000000172FB87A54D4742B", "000000000000000000000000000000000000000000000000051D78C33F75D885", 0},
		{"0000000000000000000000000000000000000000000000000DECEE03E1F793F5", "000000000000000000000000000000000000000000000000172FB87A54D4742B", "00000000000000000000000000000000000000000000000005111AB75ABF38D8", 0},
		{"0000000000000000000000000000000000000000000000000ED4E69DD06AF769", "000000000000000000000000000000000000000000000000172FB87A54D4742B", "00000000000000000000000000000000000000000000000015A0228B7FBD0741", 0},
		{"00000000000000000000000000000000000000000000000295BD93E503A1A2CD", "0000000000000000000000000000000000000000000000151AD982681C5191DB", "000000000000000000000000000000000000000000000003DA515835EDE29F5A", 0},
		{"000000000000000000000000000000000000000000000008EE9BEBE218118BF7", "0000000000000000000000000000000000000000000000151AD982681C5191DB", "00000000000000000000000000000000000000000000000D1A466A3415144C2E", 0},
		{"000000000000000000000000000000000000000000000010118C886E83656104", "0000000000000000000000000000000000000000000000151AD982681C5191DB", "000000000000000000000000000000000000000000000012C411C76C66631264", 0},
		{"00000000000000000000000000000000000000000000025C335E8CFCB013A8D9", "00000000000000000000000000000000000000000000036906CA002576FF8A0D", "000000000000000000000000000000000000000000000283953511AF14EF7E1A", 0},
		{"000000000000000000000000000000000000000000000292B60E77B6650812FC", "00000000000000000000000000000000000000000000036906CA002576FF8A0D", "00000000000000000000000000000000000000000000010C1472EC394E100B21", 0},
		{"000000000000000000000000000000000000000000000298C6A0FA943CA1FD11", "00000000000000000000000000000000000000000000036906CA002576FF8A0D", "0000000000000000000000000000000000000000000000ABC78BAE79DB5E8FE2", 0},
		{"000000000000000000000000000000000000000000000568F5083F733AB63E2F", "0000000000000000000000000000000000000000000014F5ACD47DAF70F22E91", "00000000000000000000000000000000000000000000095F54DD4CB1A4681E9A", 0},
		{"000000000000000000000000000000000000000000000946BA8C37DFD5D12F31", "0000000000000000000000000000000000000000000014F5ACD47DAF70F22E91", "000000000000000000000000000000000000000000000C2B3861243D6D04E80B", 0},
		{"000000000000000000000000000000000000000000000C6FD1D31FE7764D3823", "0000000000000000000000000000000000000000000014F5ACD47DAF70F22E91", "00000000000000000000000000000000000000000000065F4CDC1976F87ED3BF", 0},
		{"000000000000000000000000000000000000000000000EF402C2EF784DA56EC7", "000000000000000000000000000000000000000000002B246F343A228AF13C27", "00000000000000000000000000000000000000000000190C804F55810A688243", 0},
		{"00000000000000000000000000000000000000000000234221498D7B80D1BE96", "000000000000000000000000000000000000000000002B246F343A228AF13C27", "0000000000000000000000000000000000000000000007DA07DDABEBAFB16C1A", 0},
		{"00000000000000000000000000000000000000000000249180EBFD20A99805D8", "000000000000000000000000000000000000000000002B246F343A228AF13C27", "00000000000000000000000000000000000000000000112A5B320A1E49E6B94E", 0},
		{"000000000000000000000000000000000000000000655E1E2623DAA67368CDC6", "000000000000000000000000000000000000000000B5325138B68B89CBF6B36B", "0000000000000000000000000000000000000000009544AF10A657260C947656", 0},
		{"00000000000000000000000000000000000000000091994DFBC71A0334A06CA2", "000000000000000000000000000000000000000000B5325138B68B89CBF6B36B", "0000000000000000000000000000000000000000003D6F0A5E85365F83180ED5", 0},
		{"00000000000000000000000000000000000000000095A02D6577320239F51544", "000000000000000000000000000000000000000000B5325138B68B89CBF6B36B", "0000000000000000000000000000000000000000003A04E6A88BD29F8C915F38", 0},
		{"000000000000000000000000000000000000000010F3C906B3CD8242ECBFA2EE", "00000000000000000000000000000000000000002F7575D847D80BB1163B5E35", "00000000000000000000000000000000000000000EBEEF13B59DC7C8E01449E7", 0},
		{"000000000000000000000000000000000000000013CCB38C1012D094EFA6585B", "00000000000000000000000000000000000000002F7575D847D80BB1163B5E35", "00000000000000000000000000000000000000000F06DD6D8039B398AD0E9A44", 0},
		{"0000000000000000000000000000000000000000179DE03C09590DCB79D374F2", "00000000000000000000000000000000000000002F7575D847D80BB1163B5E35", "0000000000000000000000000000000000000000154F5B2E4BBEE315F58DF218", 0},
		{"00000000000000000000000000000000000000004E94660E408A206644A1EAF3", "00000000000000000000000000000000000000070BBFD0FD0BFB1BFAB1766AD3", "00000000000000000000000000000000000000041273924FEC88BA4A15B88189", 0},
		{"00000000000000000000000000000000000000033AD5F8EBF02C3359020819EE", "00000000000000000000000000000000000000070BBFD0FD0BFB1BFAB1766AD3", "000000000000000000000000000000000000000188A326301A4395043CB91A74", 0},
		{"000000000000000000000000000000000000000516B5C1FB0C7C0830A1D8B7C8", "000000000000000000000000000000000000007988E11ECCCA674CECAB3F9A4D", "00000000000000000000000000000000000000255F3DFC74BFE6F71A5162E9DF", 0},
		{"0000000000000000000000000000000000000005F6651B19AD888E9904F99109", "00000000000000000000000000000000000000070BBFD0FD0BFB1BFAB1766AD3", "000000000000000000000000000000000000000233425B369CACC15A030E61E4", 0},
		{"000000000000000000000000000000000000003745DAA466334FDDAB07AD6DC3", "000000000000000000000000000000000000007988E11ECCCA674CECAB3F9A4D", "000000000000000000000000000000000000003A25582C50018273A9B61D8B88", 0},
		{"000000000000000000000000000000000000005C4AAFA15DC141BE1BECEAB85C", "000000000000000000000000000000000000007988E11ECCCA674CECAB3F9A4D", "0000000000000000000000000000000000000054A1D08CCDB7BB10C703989B23", 0},
		{"00000000000000000000000000000000000001947BE30BE8A8C5633724A86988", "00000000000000000000000000000000000005F955625470FB84DC2C96EB5155", "00000000000000000000000000000000000003B22C88AC56D61B919F96A29B0B", 0},
		{"00000000000000000000000000000000000001B2730F6D7191C7A2CF2963631E", "00000000000000000000000000000000000005F955625470FB84DC2C96EB5155", "00000000000000000000000000000000000001A99BC48812A28417895193317C", 0},
		{"00000000000000000000000000000000000003945101821DCDCC18FAC69FC50C", "00000000000000000000000000000000000005F955625470FB84DC2C96EB5155", "00000000000000000000000000000000000000D3863434F50E55FE4BE10A54B3", 0},
		{"0000000000000000000000000000000016434AB80D50D9A2B38EFD2074E43F9B", "0000000000000000000000000000000160978DEEE0FA4648ED9E77B0182E1F59", "000000000000000000000000000000009ABA258A8E58CFEFF1D96CB3BE9CB47F", 0},
		{"00000000000000000000000000000000899A209379B27C36B15298D38ADF42FE", "0000000000000000000000000000000160978DEEE0FA4648ED9E77B0182E1F59", "00000000000000000000000000000000CD8A66569A65ACBC499AF84B08E29FAF", 0},
#ifndef USE_EMBEDDED
		{"00000000000000000000000000000001005FC2480C3F873DD7C5E258185B950D", "0000000000000000000000000000000160978DEEE0FA4648ED9E77B0182E1F59", "00000000000000000000000000000001016DC163EC0C69A8B60EC44349837C3F", 0},
		{"00000000000000000000000000001C2AB91B1E3FFFF394B4F1FCA8DF5720CF93", "0000000000000000000000000000AF153CA545A869BFBC150DFBAB1F930435F1", "00000000000000000000000000001C6DE2B5334AA826CEFE327D4DB1FDB9F488", 0},
		{"00000000000000000000000000002FBB5EC571B39C4AFA23C7BE9EB23722890A", "0000000000000000000000000000AF153CA545A869BFBC150DFBAB1F930435F1", "00000000000000000000000000004851A4DB9781313444B0E1040C62243889C8", 0},
		{"0000000000000000000000000000797432B9A3007508BCF43B6C4A74ED14C882", "0000000000000000000000000000AF153CA545A869BFBC150DFBAB1F930435F1", "00000000000000000000000000008DA8A8D5BAD01F037C22687CBBF5A395CEB2", 0},
		{"00000000000000000000000000015D94D758DA5DEE7C4E025898323B2BFAF5DF", "0000000000000000000000000002A61B3AACCFF2D254D212A45072EFEFA20DC7", "000000000000000000000000000090D3517A0222912B985A576DBF7DEE73309C", 0},
		{"000000000000000000000000000285163E074BB132F134D22294BDEFF54051F9", "0000000000000000000000000002A61B3AACCFF2D254D212A45072EFEFA20DC7", "0000000000000000000000000001D18762620DD4644114F1BA98CF8E039B3527", 0},
		{"000000000000000000000000000296158E2E5A98D7BCE3F60A2AA96BBDCFA45C", "0000000000000000000000000002A61B3AACCFF2D254D212A45072EFEFA20DC7", "000000000000000000000000000039DEAF00DD82F95C74432DBC5F3AAB0BFB0B", 0},
		{"000000000000000000001A507C677B153E09E64381EC686D570F98FB02464A67", "000000000000000000011B3A74A022F98AB287CB1971126F312BB389101D174F", "00000000000000000000A0511B7BC8A13B0FE38D61614471B35FD684FF709C9E", 0},
		{"00000000000000000000A1D86A796DC683648900B7CA7ED9E0B05C352A4D68B1", "0000000000000000000532F98BA82ADF743412BDC68DF4FC32571FCA8FB1AD9B", "0000000000000000000258141BE7D7A5E1481862EC8FE84A02CFCB3CDC0E0711", 0},
		{"00000000000000000000B6A79AD444A0C0671B1EF890C47FFC2913DEA0025365", "000000000000000000011B3A74A022F98AB287CB1971126F312BB389101D174F", "00000000000000000000C26ED6BD3B96DF8F2D402D1FC73AB6E81A3AD6C6FC40", 0},
		{"00000000000000000000F168F7D05C431203E1B61BF01BCE5275821398BE6B62", "000000000000000000011B3A74A022F98AB287CB1971126F312BB389101D174F", "00000000000000000000BD793C5837C37A90B57CB879E488052248EC9D4AE68F", 0},
		{"00000000000000000002CD9BE54727A8CE944ACF1E9714CDEA41A396E464F8EA", "0000000000000000000532F98BA82ADF743412BDC68DF4FC32571FCA8FB1AD9B", "0000000000000000000043BE28F4DAE4AF00C1744699DD06B8E4DA29F8554509", 0},
		{"000000000000000000047B19654AAB3A59DB7B0238336D971F8C96BB03E5A0BA", "0000000000000000000532F98BA82ADF743412BDC68DF4FC32571FCA8FB1AD9B", "00000000000000000001AAB1828AA5A982A2A6F314D6110880D5F7858D111A2C", 0},
		{"000000000000000000056CC15F0FDFA07B4B04BFC45AFECD01D930D639CAAF37", "0000000000000000000F471097A2C1E28348103342D73F52A7B368A102178245", "000000000000000000046D21BE28B863BFAAD4C66E174B1FD122D092CB4BB790", 0},
		{"00000000000000000005AE79805ABAF758836444704D6879D5E30783706C9686", "0000000000000000000F471097A2C1E28348103342D73F52A7B368A102178245", "00000000000000000005C43459BE13279CAD1BA47A1D99782903DE5AFC0F9837", 0},
		{"0000000000000000000601AAC2EDA9E30EBC0498FBB02AC259D6372A44385C4E", "0000000000000000000F471097A2C1E28348103342D73F52A7B368A102178245", "00000000000000000000971E94393D22F3A78A1A32B516467E6A19E4179897AA", 0},
		{"000000000000000000173B9459AF524E609682613236FFEDBE36A44ECCBFAECD", "0000000000000000009AEA403CC86257493A47B011D4785BB3FDEEC172E16C95", "000000000000000000330814D19E417F249F36A8F9B74D371CC6DD1DA8C29130", 0},
		{"0000000000000000005AB87C4CE0AE14B28634B84D7052846C078CDA85965F4F", "0000000000000000009AEA403CC86257493A47B011D4785BB3FDEEC172E16C95", "0000000000000000006EE50EF1A18C1BF8E1252B16A4B53BD5E1834F46E79D6C", 0},
		{"00000000000000000089FFD79C887751CC57A2BD9DB3CC7E806CBEF8154AA7F0", "0000000000000000009AEA403CC86257493A47B011D4785BB3FDEEC172E16C95", "0000000000000000002F4497A79C8D91D479AFD74F00517C1A8ADFC55D456A00", 0},
		{"0000000000000003868D14868FADB371FF02EA0727E1039DB3C980B8008B35C6", "000000000000000EF1CA1C30D13F0E75D0654FC9C0B25836774A83A6D145A65D", "000000000000000AC6A07090D7A5DEA9D83E14D980147641142301F36C5307EC", 0},
		{"0000000000000005F3ADD7C360C78019F1BD71DA0A75B84C70E430DFB299A1A3", "000000000000000EF1CA1C30D13F0E75D0654FC9C0B25836774A83A6D145A65D", "0000000000000003ED3C26410B3952CF69776E13A07CCC6618A4FE4C42847556", 0},
		{"00000000000000098952A70960A7C78FE7E924D48DEDCE43EE9E6DBCBBDB95F4", "000000000000000EF1CA1C30D13F0E75D0654FC9C0B25836774A83A6D145A65D", "000000000000000C6D5BC0FDC35798E0D1090C8E055FB59A44A2E51A0EA62E95", 0},
		{"0000000000000017ED7632E0D479EAA27DB8063B24C48AA53DCF5BE5B50E47E6", "0000000000000077C90315A5E82FD3F2614EE8580AB24D6CF6F71879C5A1AD61", "000000000000005F5A816B9A126F8F2E26C2B3B8E126ABAE0EDE919C43FCA1E7", 0},
		{"000000000000003074438A9E94BFE2FAEE9EDE1943FB906002A598C3C19212B3", "0000000000000077C90315A5E82FD3F2614EE8580AB24D6CF6F71879C5A1AD61", "0000000000000026BFCDF00E8AC90DCBE1BDC5E4487DE5544902E6BB8FBD22B6", 0},
		{"00000000000000447ED29FD899037B711334C4D35FF394845069325BECB1931A", "0000000000000077C90315A5E82FD3F2614EE8580AB24D6CF6F71879C5A1AD61", "0000000000000038AF2E9E173031BAD5EAE90851B209C72EC61DA377BB8353FC", 0},
		{"00000000000000B77FD147681FDFC372FE0D9FC308656EBAB1111DD542884AF9", "00000000000018DE7A73426BA32C2FE958083681C717047C06FE8C993674D9CB", "00000000000015A07CFBA9840861CDF7A3AC8E49003AA73AE788F1134EDAED43", 0},
		{"0000000000000348AF484312A97F6824AF8C304DE500984D17573C5500564042", "000000000000177389D552D0C927478AD3E4A82FA1F247BF7672ED21337C2991", "00000000000006804B8F91A90D1E6082C6D913DBCDF9A7EE7FD8F2F190CA11C8", 0},
		{"00000000000003D261ACF86A65AF0014F4E7FBA6302D545D1243852A9337AF12", "00000000000018DE7A73426BA32C2FE958083681C717047C06FE8C993674D9CB", "00000000000006A7FA5CCD5330746237CBD97238259E88CA335F235F2F9E0B04", 0},
		{"0000000000000509AEA53884538EC0861775D7B5EF907DB0C236AFAE9A79F881", "00000000000018DE7A73426BA32C2FE958083681C717047C06FE8C993674D9CB", "000000000000129B6240B5794D730510FE027A3876E357AD0DFAB935AF86D37E", 0},
		{"00000000000008A56008816204F89AA67F94C08FB4EF17A90DC89AB53BAFD021", "000000000000177389D552D0C927478AD3E4A82FA1F247BF7672ED21337C2991", "00000000000004DC07B7322E1323FDCE44193637DEB36F8CF546175CE2A09298", 0},
		{"0000000000000D952CB12354B3DD190E8F72D721AEA6AFD3AE467E3E15C8D7EF", "000000000000177389D552D0C927478AD3E4A82FA1F247BF7672ED21337C2991", "00000000000011E42921C1EE678934D7B0E16CA63E42B7B0256BA20B38158AA0", 0},
		{"000000000022FB8E962951C7068D05DB5D047CE6F30E38E5CA88CBA6661A9354", "00000000031AF60245AD37EEC3F03A1DC31D08C51F155C696A9DBEFCE4584403", "00000000019DAE91CED842C75343EB86FCDDF21DE188A627BC4AEA27FAC9C54D", 0},
		{"000000000040401D4286086D08A867452DA70F8B4797C11DF81747D85CDF9EC8", "00000000031AF60245AD37EEC3F03A1DC31D08C51F155C696A9DBEFCE4584403", "0000000000A2A501692829FB2B6D5F90852F382A2231D178D2C6D871C94CEED9", 0},
		{"000000000103078B6664F72C385390DB6A59DD41CFAC2F9E353AE057977BEF06", "00000000031AF60245AD37EEC3F03A1DC31D08C51F155C696A9DBEFCE4584403", "0000000002BE153C0F28C857F4F0EBB7172F343FF9A96CF2FD3C6405818EE7A4", 0},
		{"00000000011873C6EC61A64044724725F41E17E3EA0405AC4C342554F606FF07", "0000000002821D4F811CD47467C5D72405C137AC84E851C3A058937AC391753F", "00000000001508C2DA5590C5AB9AF4070D1D2DDE3FE923B3921056B5DE2DE85A", 0},
		{"0000000001BEC00C0972CC7C71BFBBB6A46807D344E1A324DA16E5C7720BA5D4", "0000000002821D4F811CD47467C5D72405C137AC84E851C3A058937AC391753F", "00000000006E503ECD14ECB690319E7B271F4C05D5E590B67C0D4B652B0A713F", 0},
		{"0000000001E265A3E6078B1833B3B7EB51710ED7F9EB585B27E940ABCF18EAD9", "0000000002821D4F811CD47467C5D72405C137AC84E851C3A058937AC391753F", "00000000020CEF933720E69779B16C949C3BCD7CBC8FE809A572097C24DF9596", 0},
		{"000002A45E256282090BC6AEAAF1E622F4321ACD4FCE079C3123EB7832C52B01", "00001C634EB2ECC0B389A5248EEB74A207A13EA970904505E0F237E446B2655B", "000014BC1AE1A795B85E1DADE7E616D04E8F2F42B5ABD09696889D7477EE1DBB", 0},
		{"00000F668C318D950B49B581914226752A4B8FE116C0045AFC68F5EE1FAF6C70", "00001C634EB2ECC0B389A5248EEB74A207A13EA970904505E0F237E446B2655B", "00000A2E067C067F5EAC89178A62EE74D4D0951915E3DDB4CF04C2067F9BD988", 0},
		{"00001543676FD264D9E0F790335C9DB0F582CAC7A79F7EB59B24AAD95A890BDA", "00001C634EB2ECC0B389A5248EEB74A207A13EA970904505E0F237E446B2655B", "00001B615B57D0963DB6990BC9E918F7E3450690C2917C70189F08D45A466205", 0},
		{"004AC901B4AD0DACE2C837EFBAE784F7661647919B5D11F694FD2694352A1845", "030F5ABABFE96C57D7FC25F26D06113682EF1CC4CB1A3F086CDFEAF5F5928127", "023C3FD20F02832E3DFCACEC409377D3E36558386212942C14B127412889D80A", 0},
		{"0278D3A67A0D2DCE7058A2FA7EF96AAE8864468FB3106A6C3A6DD0815912CE12", "030F5ABABFE96C57D7FC25F26D06113682EF1CC4CB1A3F086CDFEAF5F5928127", "0006C33F179667CE775924B8B71C56B926541DBDCEFF2BD463F7018F8C060413", 0},
		{"02FEC3A86C904DF34FF48B889D3F2271EB270F8AB63EEECB194971DE4218DA79", "030F5ABABFE96C57D7FC25F26D06113682EF1CC4CB1A3F086CDFEAF5F5928127", "02F75EC2D3E5387C9F83D5ABC7CED84DA4E127D8BFB4A17D67A1565DE292EF56", 0},
		{"062DE676638605D0F9DEB955929D1939ECF377F1F25B5F3886285AA03710B62C", "4554F111E81655A9D42C332C0D436A4E39F53CC999A5AB111B68D931A1124519", "1D86E0291807C9C84D0C7B2F77E6F6F362D7BEEFBE174E2007C741633195EC45", 0},
		{"15D2E2BF94051026B54336F6AC15D6A9C1E171F84D5DED7D4EAB86861EDBFB5A", "4554F111E81655A9D42C332C0D436A4E39F53CC999A5AB111B68D931A1124519", "03345D5426F3B38BFAA2DC1DC4FFB4906724B32A4882C728022579B4FE6997A4", 0},
		{"36062BC132B5ECBFDBCD75C1822115B2B93977BB2E5BC7FC8B8A51ABE1D4D081", "4554F111E81655A9D42C332C0D436A4E39F53CC999A5AB111B68D931A1124519", "25D9FA60C4DD3D67F9573BC2BA866F6B2B295DF1294A47006A5406E7A476FD24", 0},
		{"C0EE5C43AADB01D95B8E7C300DFEDCF2487FE21F02679C16FB44A1B8FE7F8D36", "E5A5FE281AED3487822FFD2442A2484A244A168C0B59215AC3C686C9B650EC7B", "E32701CC43A21E8879595EDA986F1B277EBFA9CA7CA3CDB39B46A361C9588FD0", 0},
		{"C7DDA628A3D4EE89D5E85E52489E2A0BD4FA7978667FE5A7D8635A3B39E32D52", "E5A5FE281AED3487822FFD2442A2484A244A168C0B59215AC3C686C9B650EC7B", "1E2E6E2076AA8A9B7D957D43FEAB45A7E464B2DE9EA93280C94FDAF3759EB93C", 0},
		{"CB1091D01C0A0019534FE60409F37B0AB199957E307DCD34375AF0EB28027617", "E5A5FE281AED3487822FFD2442A2484A244A168C0B59215AC3C686C9B650EC7B", "196BC84DA4D52061821DBE6E76F91F616AED7DE5F9AD225BC607602A69DBBCB7", 0},
#endif
	}};
	for (const TernaryCase &tc : cases) {
		Uint256 x(tc.x);
		x.reciprocal(Uint256(tc.y));
		assert(x == Uint256(tc.z));
	}
}


TEST(uint256, replace_and_swap) {
	const size_t CASE_SIZE = 13U;
	const std::array<BinaryCase, CASE_SIZE> cases{{
		{"0000000000000000000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"0000000000000000000000000000000000000000000000000000000000000000", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", "0000000000000000000000000000000000000000000000000000000000000000"},
		{"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"},
		{"0000000000000100000000000000000000000000000000000000000000000000", "0000000000000000000000000000000000000000000000000000000000000008"},
		{"12041F8A08B389EA4E0E06AF09E0929EFB8B1B86DEC57D6FDEB3D9D115F6248B", "FA84D9FC056EFB0FB361E3E1BE0AB792EB1E264F64BC98E430280984076F48E9"},
		{"EB3031D2E93D9D6A5ACC06749CB81E22AA356A2CC993E9A6248EBB894B6433EB", "38C412BD1E74230EB2239752396C184138B71A2098B36982A7ABE540F87F9DAA"},
		{"0E1AA5A5161DF93A4137BCDCD82E4F0CD0915CB90C5CD06470240F6A2180B8A1", "7B0CCF9E16381BC87FF7AABD209C9245A78EB029241FE3C38723BA8E7E5BB63E"},
		{"0754D05EAE2A7F834400DC2CAF9DE9703BF68DE8239E0250E09922FB4948661E", "29F0DBA4ED66F5013A1C73B713CEB46B243769E7CDF6BA535E7CFAEC1708F998"},
		{"16E6F9A0E9FE2BC49E0042BD66E6977B96BEECD1ACFC9D8A59DDB56E8B2DA33E", "8FC4E7FED2002227C5C8816195D2FE95968968085522E147B64914D365A763EC"},
		{"4F82285DE18823579D43E3570F7036A2DCC9DED10556DDD21D98B5AB54016D99", "67C27D521E4ED8C5614D954C270E342EE6C1CF39147411F6D096CBA0A9C401BB"},
		{"56BBC0162E1402A5F93AB30DB97508716C40DE99BB5AEB5445B0BFE8C20499E7", "A9E5B2315A2E457A9BDBBB2FDDCBEE9BCD35E6DC9AB782A28828302D3909E1EF"},
		{"CADEB692CA96963BC61BC9771164FAE51C86DD1635840FF60D4358CF67323DAD", "413119D653C67245906E421F502A80C12157155ECE0BF9DA48F6A3BF961C7A91"},
	}};
	for (const BinaryCase &tc : cases) {
		Uint256 a(tc.x);
		Uint256 b(tc.y);
		Uint256 x(tc.x);
		Uint256 y(tc.y);
		x.replace(b, 0);
		assert(x == a);
		x.replace(b, 1);
		assert(x == b);
		y.replace(a, 1);
		assert(y == a);
		x.swap(y, 0);
		assert(x == b && y == a);
		x.swap(y, 1);
		assert(x == a && y == b);
	}
}


TEST(uint256, constructor_bytes) {
	const std::uint8_t b[32] = {
		0x03, 0x4D, 0x03, 0x33,
		0x2D, 0xCE, 0x3A, 0x5F,
		0xA5, 0xCA, 0x65, 0x3B,
		0x54, 0x33, 0x5E, 0x14,
		0x81, 0x38, 0xB3, 0xA1,
		0x3C, 0x27, 0x95, 0xA3,
		0x48, 0xE6, 0x9E, 0xFE,
		0xA7, 0xCA, 0xC5, 0x16,
	};
	Uint256 x(b);
	assert(x.value[0] == UINT32_C(0xA7CAC516));
	assert(x.value[1] == UINT32_C(0x48E69EFE));
	assert(x.value[4] == UINT32_C(0x54335E14));
	assert(x.value[7] == UINT32_C(0x034D0333));
}

TEST(uint256, get_big_endian_bytes) {
	Uint256 x("292180DB102E1F14BB371F9016E785D2C83B59AA29497BA9E595025D9373A038");
	std::uint8_t b[32];
	x.getBigEndianBytes(b);
	assert(b[ 0] == 0x29);
	assert(b[ 1] == 0x21);
	assert(b[ 2] == 0x80);
	assert(b[ 4] == 0x10);
	assert(b[ 7] == 0x14);
	assert(b[22] == 0x7B);
	assert(b[29] == 0x73);
	assert(b[31] == 0x38);
}