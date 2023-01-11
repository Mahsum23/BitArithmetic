#include <vector>
#include <iostream>
#include <string>
#include <utility>

#include "Tester.hpp"

struct Moves
{
	size_t count = 0;
	uint64_t bits = 0;

	std::string ToString() const
	{
		return std::to_string(count) + '\n' + std::to_string(bits);
	}
};

size_t CountOneBits(uint64_t n)
{
	size_t res = 0;
	while (n)
	{
		n &= (n - 1);
		++res;
	}
	return res;
}

size_t CountOneBitsRec(uint64_t n)
{
	if (n == 0) return 0ull;
	return CountOneBitsRec(n >> 1) + n % 2;
}

size_t CountOneBitsCache(uint64_t n)
{
	static std::vector<uint8_t> cache = []()
	{
		std::vector<uint8_t> cache(UINT16_MAX);
		for (uint16_t i = 1; i < cache.size(); ++i)
		{
			cache[i] = cache[i & (i - 1)] + 1;
		}
		return cache;
	}();
	
	if (n == 0) return 0ull;
	return CountOneBitsCache(n >> 16) + cache[static_cast<uint16_t>(n)];

}

Moves KingMoves(int pos)
{
	uint64_t bitpos = 1ull << pos;

	uint64_t bits1 = 0;
	bits1 |= (bitpos >> 1);
	bits1 |= (bitpos << 7);
	bits1 |= (bitpos >> 9);
	bits1 &= 0x7f7f7f7f7f7f7f7f;
	
	uint64_t bits2 = 0;
	bits2 |= (bitpos << 1);
	bits2 |= (bitpos >> 7);
	bits2 |= (bitpos << 9);
	bits2 &= 0xfefefefefefefefe;

	bits2 |= (bitpos >> 8);
	bits2 |= (bitpos << 8);

	return { __popcnt64(bits1 | bits2), bits1 | bits2};
}

Moves KnightMoves(int pos)
{
	uint64_t bitpos = 1ull << pos;
	uint64_t moves = 0;
	uint64_t nA = 0xFeFeFeFeFeFeFeFe;
	uint64_t nAB = 0xFcFcFcFcFcFcFcFc;
	uint64_t nH = 0x7f7f7f7f7f7f7f7f;
	uint64_t nGH = 0x3f3f3f3f3f3f3f3f;
	moves = nGH & (bitpos << 6 | bitpos >> 10)
		| nH & (bitpos << 15 | bitpos >> 17)
		| nA & (bitpos << 17 | bitpos >> 15)
		| nAB & (bitpos << 10 | bitpos >> 6);

	return { __popcnt64(moves), moves };
}

Moves RookMoves(int pos)
{
	uint64_t bitpos = 1ull << pos;

	uint64_t bits1 = 0;
	for (int i = 1; i < 8; ++i)
	{
		bits1 |= (bitpos >> 8 * i);
		bits1 |= (bitpos << 8 * i);
	}

	uint64_t bits2 = bitpos;
	uint64_t mask = 0x00000000000000FF;
	while (!(bits2 & mask))
	{
		mask <<= 8;
	}
	bits2 ^= mask;

	return { __popcnt64(bits1 | bits2), bits1 | bits2 };
}

Moves BishopMoves(int pos)
{
	uint64_t bitpos = 1ull << pos;
	uint64_t up_left_border = 0xff01010101010101;
	uint64_t down_left_border = 0x1010101010101ff;
	uint64_t up_right_border = 0xff80808080808080;
	uint64_t down_right_border = 0x80808080808080ff;

	uint64_t bits = bitpos;
	while (!(bitpos & up_left_border))
	{
		bitpos <<= 7;
		bits |= bitpos;
	}

	bitpos = 1ull << pos;
	while (!(bits & down_right_border))
	{
		bitpos >>= 7;
		bits |= bitpos;
	}

	bitpos = 1ull << pos;
	while (!(bitpos & up_right_border))
	{
		bitpos <<= 9;
		bits |= bitpos;
	}

	bitpos = 1ull << pos;
	while (!(bits & down_left_border))
	{
		bitpos >>= 9;
		bits |= bitpos;
	}
	bits ^= (1ull << pos);

	return { __popcnt64(bits), bits };
}

Moves QueenMoves(int pos)
{
	uint64_t bitpos = 1ull << pos;
	uint64_t bits = RookMoves(pos).bits | BishopMoves(pos).bits;

	return { __popcnt64(bits), bits };
}


int main()
{

	{
		auto KingMovesStr = [](std::string pos) { return KingMoves(std::stoi(pos)).ToString(); };
		Tester<std::string(std::string), 1> king_moves_tester("king"s, "KingMoves", KingMovesStr);
		king_moves_tester.Run();
	}

	{
		auto KnightMovesStr = [](std::string pos) { return KnightMoves(std::stoi(pos)).ToString(); };
		Tester<std::string(std::string), 1> knight_moves_tester("knight"s, "KnightMoves", KnightMovesStr);
		knight_moves_tester.Run();
	}

	{
		auto RookMovesStr = [](std::string pos) { return RookMoves(std::stoi(pos)).ToString(); };
		Tester<std::string(std::string), 1> rook_moves_tester("rook"s, "RookMoves", RookMovesStr);
		rook_moves_tester.Run();
	}

	{
		auto BishopMovesStr = [](std::string pos) { return BishopMoves(std::stoi(pos)).ToString(); };
		Tester<std::string(std::string), 1> bishop_moves_tester("bishop"s, "BishopMoves", BishopMovesStr);
		bishop_moves_tester.Run();
	}

	{
		auto QueenMovesStr = [](std::string pos) { return QueenMoves(std::stoi(pos)).ToString(); };
		Tester<std::string(std::string), 1> queen_moves_tester("queen"s, "QueenMoves", QueenMovesStr);
		queen_moves_tester.Run();
	}

	std::cin.get();
}