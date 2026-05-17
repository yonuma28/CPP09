#include "PmergeMe.hpp"

#include <climits>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace
{
	struct VectorPair
	{
		int small;
		int large;
		std::size_t id;
	};

	struct DequePair
	{
		int small;
		int large;
		std::size_t id;
	};

	struct VectorNode
	{
		int value;
		std::size_t pairId;
		bool isLarge;
	};

	struct DequeNode
	{
		int value;
		std::size_t pairId;
		bool isLarge;
	};

	struct VectorPendingPair
	{
		VectorPair pair;
		std::size_t partnerId;
	};

	struct DequePendingPair
	{
		DequePair pair;
		std::size_t partnerId;
	};

	std::vector<std::size_t> buildJacobsthalOrderVector(std::size_t pairCount)
	{
		std::vector<std::size_t> order;
		std::size_t previousJacob = 1;
		std::size_t currentJacob = 3;
		std::size_t previousBoundary = 1;

		if (pairCount <= 1)
			return order;
		while (previousBoundary < pairCount)
		{
			std::size_t upper = currentJacob;
			std::size_t nextJacob;

			if (upper > pairCount)
				upper = pairCount;
			for (std::size_t index = upper; index > previousBoundary; --index)
				order.push_back(index - 1);
			nextJacob = currentJacob + (2 * previousJacob);
			previousBoundary = currentJacob;
			previousJacob = currentJacob;
			currentJacob = nextJacob;
		}
		return order;
	}

	std::deque<std::size_t> buildJacobsthalOrderDeque(std::size_t pairCount)
	{
		std::deque<std::size_t> order;
		std::size_t previousJacob = 1;
		std::size_t currentJacob = 3;
		std::size_t previousBoundary = 1;

		if (pairCount <= 1)
			return order;
		while (previousBoundary < pairCount)
		{
			std::size_t upper = currentJacob;
			std::size_t nextJacob;

			if (upper > pairCount)
				upper = pairCount;
			for (std::size_t index = upper; index > previousBoundary; --index)
				order.push_back(index - 1);
			nextJacob = currentJacob + (2 * previousJacob);
			previousBoundary = currentJacob;
			previousJacob = currentJacob;
			currentJacob = nextJacob;
		}
		return order;
	}

	std::size_t findVectorLargePosition(const std::vector<VectorNode>& chain, std::size_t pairId)
	{
		for (std::size_t i = 0; i < chain.size(); ++i)
		{
			if (chain[i].isLarge && chain[i].pairId == pairId)
				return i;
		}
		return chain.size();
	}

	std::size_t findDequeLargePosition(const std::deque<DequeNode>& chain, std::size_t pairId)
	{
		for (std::size_t i = 0; i < chain.size(); ++i)
		{
			if (chain[i].isLarge && chain[i].pairId == pairId)
				return i;
		}
		return chain.size();
	}

	void insertVectorNodeByBound(std::vector<VectorNode>& chain, const VectorNode& node,
		std::size_t high, int& comparisonCount)
	{
		std::size_t low = 0;

		while (low < high)
		{
			std::size_t mid = low + (high - low) / 2;

			++comparisonCount;
			if (chain[mid].value < node.value)
				low = mid + 1;
			else
				high = mid;
		}
		chain.insert(chain.begin() + static_cast<std::vector<VectorNode>::difference_type>(low), node);
	}

	void insertDequeNodeByBound(std::deque<DequeNode>& chain, const DequeNode& node,
		std::size_t high, int& comparisonCount)
	{
		std::size_t low = 0;

		while (low < high)
		{
			std::size_t mid = low + (high - low) / 2;

			++comparisonCount;
			if (chain[mid].value < node.value)
				low = mid + 1;
			else
				high = mid;
		}
		chain.insert(chain.begin() + static_cast<std::deque<DequeNode>::difference_type>(low), node);
	}

	void insertVectorPairByBound(std::vector<VectorPair>& chain, const VectorPair& pair,
		std::size_t high, int& comparisonCount)
	{
		std::size_t low = 0;

		while (low < high)
		{
			std::size_t mid = low + (high - low) / 2;

			++comparisonCount;
			if (chain[mid].large < pair.large)
				low = mid + 1;
			else
				high = mid;
		}
		chain.insert(chain.begin() + static_cast<std::vector<VectorPair>::difference_type>(low), pair);
	}

	void insertDequePairByBound(std::deque<DequePair>& chain, const DequePair& pair,
		std::size_t high, int& comparisonCount)
	{
		std::size_t low = 0;

		while (low < high)
		{
			std::size_t mid = low + (high - low) / 2;

			++comparisonCount;
			if (chain[mid].large < pair.large)
				low = mid + 1;
			else
				high = mid;
		}
		chain.insert(chain.begin() + static_cast<std::deque<DequePair>::difference_type>(low), pair);
	}

	VectorPair findVectorPendingByPartnerId(const std::vector<VectorPendingPair>& pendingPairs, std::size_t partnerId)
	{
		for (std::size_t i = 0; i < pendingPairs.size(); ++i)
		{
			if (pendingPairs[i].partnerId == partnerId)
				return pendingPairs[i].pair;
		}
		return VectorPair();
	}

	DequePair findDequePendingByPartnerId(const std::deque<DequePendingPair>& pendingPairs, std::size_t partnerId)
	{
		for (std::size_t i = 0; i < pendingPairs.size(); ++i)
		{
			if (pendingPairs[i].partnerId == partnerId)
				return pendingPairs[i].pair;
		}
		return DequePair();
	}

	std::vector<VectorPair> sortVectorPairsByLarge(const std::vector<VectorPair>& input,
		int& comparisonCount)
	{
		std::vector<VectorPendingPair> pendingPairs;
		std::vector<VectorPair> largePairs;
		std::vector<VectorPair> sortedLargePairs;
		std::vector<VectorPair> mainChain;
		std::vector<std::size_t> insertionOrder;
		bool hasLeftover = false;
		VectorPair leftover;
		std::size_t i;

		if (input.size() <= 1)
			return input;
		for (i = 0; i + 1 < input.size(); i += 2)
		{
			VectorPendingPair pending;

			++comparisonCount;
			if (input[i].large <= input[i + 1].large)
			{
				pending.pair = input[i];
				pending.partnerId = input[i + 1].id;
				pendingPairs.push_back(pending);
				largePairs.push_back(input[i + 1]);
			}
			else
			{
				pending.pair = input[i + 1];
				pending.partnerId = input[i].id;
				pendingPairs.push_back(pending);
				largePairs.push_back(input[i]);
			}
		}
		if (i < input.size())
		{
			hasLeftover = true;
			leftover = input[i];
		}
		sortedLargePairs = sortVectorPairsByLarge(largePairs, comparisonCount);
		mainChain.push_back(findVectorPendingByPartnerId(pendingPairs, sortedLargePairs[0].id));
		for (i = 0; i < sortedLargePairs.size(); ++i)
			mainChain.push_back(sortedLargePairs[i]);
		insertionOrder = buildJacobsthalOrderVector(sortedLargePairs.size()
				+ (hasLeftover ? 1 : 0));
		for (i = 0; i < insertionOrder.size(); ++i)
		{
			std::size_t partnerIndex = insertionOrder[i];
			std::size_t partnerPos;

			if (hasLeftover && partnerIndex == sortedLargePairs.size())
			{
				insertVectorPairByBound(mainChain, leftover, mainChain.size(), comparisonCount);
				continue;
			}
			VectorPair partnerSmall = findVectorPendingByPartnerId(pendingPairs,
					sortedLargePairs[partnerIndex].id);

			for (partnerPos = 0; partnerPos < mainChain.size(); ++partnerPos)
			{
				if (mainChain[partnerPos].id == sortedLargePairs[partnerIndex].id)
					break;
			}
			insertVectorPairByBound(mainChain, partnerSmall, partnerPos, comparisonCount);
		}
		return mainChain;
	}

	std::deque<DequePair> sortDequePairsByLarge(const std::deque<DequePair>& input,
		int& comparisonCount)
	{
		std::deque<DequePendingPair> pendingPairs;
		std::deque<DequePair> largePairs;
		std::deque<DequePair> sortedLargePairs;
		std::deque<DequePair> mainChain;
		std::deque<std::size_t> insertionOrder;
		bool hasLeftover = false;
		DequePair leftover;
		std::size_t i;

		if (input.size() <= 1)
			return input;
		for (i = 0; i + 1 < input.size(); i += 2)
		{
			DequePendingPair pending;

			++comparisonCount;
			if (input[i].large <= input[i + 1].large)
			{
				pending.pair = input[i];
				pending.partnerId = input[i + 1].id;
				pendingPairs.push_back(pending);
				largePairs.push_back(input[i + 1]);
			}
			else
			{
				pending.pair = input[i + 1];
				pending.partnerId = input[i].id;
				pendingPairs.push_back(pending);
				largePairs.push_back(input[i]);
			}
		}
		if (i < input.size())
		{
			hasLeftover = true;
			leftover = input[i];
		}
		sortedLargePairs = sortDequePairsByLarge(largePairs, comparisonCount);
		mainChain.push_back(findDequePendingByPartnerId(pendingPairs, sortedLargePairs[0].id));
		for (i = 0; i < sortedLargePairs.size(); ++i)
			mainChain.push_back(sortedLargePairs[i]);
		insertionOrder = buildJacobsthalOrderDeque(sortedLargePairs.size()
				+ (hasLeftover ? 1 : 0));
		for (i = 0; i < insertionOrder.size(); ++i)
		{
			std::size_t partnerIndex = insertionOrder[i];
			std::size_t partnerPos;

			if (hasLeftover && partnerIndex == sortedLargePairs.size())
			{
				insertDequePairByBound(mainChain, leftover, mainChain.size(), comparisonCount);
				continue;
			}
			DequePair partnerSmall = findDequePendingByPartnerId(pendingPairs,
					sortedLargePairs[partnerIndex].id);

			for (partnerPos = 0; partnerPos < mainChain.size(); ++partnerPos)
			{
				if (mainChain[partnerPos].id == sortedLargePairs[partnerIndex].id)
					break;
			}
			insertDequePairByBound(mainChain, partnerSmall, partnerPos, comparisonCount);
		}
		return mainChain;
	}
}

PmergeMe::PmergeMe()
	: _vectorComparisonCount(0), _dequeComparisonCount(0)
{
}

PmergeMe::PmergeMe(const PmergeMe& other)
	: _vectorInput(other._vectorInput), _dequeInput(other._dequeInput),
		_vectorComparisonCount(other._vectorComparisonCount),
		_dequeComparisonCount(other._dequeComparisonCount)
{
}

PmergeMe& PmergeMe::operator=(const PmergeMe& other)
{
	if (this != &other)
	{
		_vectorInput = other._vectorInput;
		_dequeInput = other._dequeInput;
		_vectorComparisonCount = other._vectorComparisonCount;
		_dequeComparisonCount = other._dequeComparisonCount;
	}
	return *this;
}

PmergeMe::~PmergeMe()
{
}

bool PmergeMe::isValidPositiveIntegerString(const std::string& s)
{
	std::size_t i;

	if (s.empty())
		return false;
	if (s[0] == '0')
		return false;
	for (i = 0; i < s.length(); ++i)
	{
		if (s[i] < '0' || s[i] > '9')
			return false;
	}
	return true;
}

int PmergeMe::parsePositiveInt(const std::string& s)
{
	std::istringstream iss(s);
	long value;

	if (!isValidPositiveIntegerString(s))
		throw std::runtime_error("Error");
	iss >> value;
	if (iss.fail() || !iss.eof() || value <= 0 || value > INT_MAX)
		throw std::runtime_error("Error");
	return static_cast<int>(value);
}

void PmergeMe::parseInput(int argc, char** argv)
{
	int i;

	if (argc < 2)
		throw std::runtime_error("Error");
	_vectorInput.clear();
	_dequeInput.clear();
	for (i = 1; i < argc; ++i)
	{
		int value = parsePositiveInt(argv[i]);

		_vectorInput.push_back(value);
		_dequeInput.push_back(value);
	}
}

void PmergeMe::binaryInsertVector(std::vector<int>& mainChain, int value) const
{
	std::size_t low = 0;
	std::size_t high = mainChain.size();

	while (low < high)
	{
		std::size_t mid = low + (high - low) / 2;

		++_vectorComparisonCount;
		if (mainChain[mid] < value)
			low = mid + 1;
		else
			high = mid;
	}
	mainChain.insert(mainChain.begin() + static_cast<std::vector<int>::difference_type>(low), value);
}

void PmergeMe::binaryInsertDeque(std::deque<int>& mainChain, int value) const
{
	std::size_t low = 0;
	std::size_t high = mainChain.size();

	while (low < high)
	{
		std::size_t mid = low + (high - low) / 2;

		++_dequeComparisonCount;
		if (mainChain[mid] < value)
			low = mid + 1;
		else
			high = mid;
	}
	mainChain.insert(mainChain.begin() + static_cast<std::deque<int>::difference_type>(low), value);
}

std::vector<int> PmergeMe::mergeInsertSortVector(const std::vector<int>& input) const
{
	std::vector<VectorPair> pairs;
	std::vector<VectorPair> sortedPairs;
	std::vector<VectorNode> chain;
	std::vector<std::size_t> insertionOrder;
	std::vector<int> result;
	bool hasLeftover = false;
	int leftover = 0;
	std::size_t i;

	if (input.size() <= 1)
		return input;
	for (i = 0; i + 1 < input.size(); i += 2)
	{
		VectorPair pair;

		++_vectorComparisonCount;
		if (input[i] <= input[i + 1])
		{
			pair.small = input[i];
			pair.large = input[i + 1];
		}
		else
		{
			pair.small = input[i + 1];
			pair.large = input[i];
		}
		pair.id = pairs.size();
		pairs.push_back(pair);
	}
	if (i < input.size())
	{
		hasLeftover = true;
		leftover = input[i];
	}
	sortedPairs = sortVectorPairsByLarge(pairs, _vectorComparisonCount);
	chain.push_back(VectorNode());
	chain.back().value = sortedPairs[0].small;
	chain.back().pairId = sortedPairs[0].id;
	chain.back().isLarge = false;
	for (i = 0; i < sortedPairs.size(); ++i)
	{
		VectorNode node;

		node.value = sortedPairs[i].large;
		node.pairId = sortedPairs[i].id;
		node.isLarge = true;
		chain.push_back(node);
	}
	insertionOrder = buildJacobsthalOrderVector(sortedPairs.size()
			+ (hasLeftover ? 1 : 0));
	for (i = 0; i < insertionOrder.size(); ++i)
	{
		std::size_t pairIndex = insertionOrder[i];
		std::size_t partnerPos;
		VectorNode node;

		if (hasLeftover && pairIndex == sortedPairs.size())
		{
			node.value = leftover;
			node.pairId = sortedPairs.size();
			node.isLarge = false;
			insertVectorNodeByBound(chain, node, chain.size(), _vectorComparisonCount);
			continue;
		}
		partnerPos = findVectorLargePosition(chain, sortedPairs[pairIndex].id);
		node.value = sortedPairs[pairIndex].small;
		node.pairId = sortedPairs[pairIndex].id;
		node.isLarge = false;
		insertVectorNodeByBound(chain, node, partnerPos, _vectorComparisonCount);
	}
	result.reserve(chain.size() + (hasLeftover ? 1 : 0));
	for (i = 0; i < chain.size(); ++i)
		result.push_back(chain[i].value);
	return result;
}

std::deque<int> PmergeMe::mergeInsertSortDeque(const std::deque<int>& input) const
{
	std::deque<DequePair> pairs;
	std::deque<DequePair> sortedPairs;
	std::deque<DequeNode> chain;
	std::deque<std::size_t> insertionOrder;
	std::deque<int> result;
	bool hasLeftover = false;
	int leftover = 0;
	std::size_t i;

	if (input.size() <= 1)
		return input;
	for (i = 0; i + 1 < input.size(); i += 2)
	{
		DequePair pair;

		++_dequeComparisonCount;
		if (input[i] <= input[i + 1])
		{
			pair.small = input[i];
			pair.large = input[i + 1];
		}
		else
		{
			pair.small = input[i + 1];
			pair.large = input[i];
		}
		pair.id = pairs.size();
		pairs.push_back(pair);
	}
	if (i < input.size())
	{
		hasLeftover = true;
		leftover = input[i];
	}
	sortedPairs = sortDequePairsByLarge(pairs, _dequeComparisonCount);
	chain.push_back(DequeNode());
	chain.back().value = sortedPairs[0].small;
	chain.back().pairId = sortedPairs[0].id;
	chain.back().isLarge = false;
	for (i = 0; i < sortedPairs.size(); ++i)
	{
		DequeNode node;

		node.value = sortedPairs[i].large;
		node.pairId = sortedPairs[i].id;
		node.isLarge = true;
		chain.push_back(node);
	}
	insertionOrder = buildJacobsthalOrderDeque(sortedPairs.size()
			+ (hasLeftover ? 1 : 0));
	for (i = 0; i < insertionOrder.size(); ++i)
	{
		std::size_t pairIndex = insertionOrder[i];
		std::size_t partnerPos;
		DequeNode node;

		if (hasLeftover && pairIndex == sortedPairs.size())
		{
			node.value = leftover;
			node.pairId = sortedPairs.size();
			node.isLarge = false;
			insertDequeNodeByBound(chain, node, chain.size(), _dequeComparisonCount);
			continue;
		}
		partnerPos = findDequeLargePosition(chain, sortedPairs[pairIndex].id);
		node.value = sortedPairs[pairIndex].small;
		node.pairId = sortedPairs[pairIndex].id;
		node.isLarge = false;
		insertDequeNodeByBound(chain, node, partnerPos, _dequeComparisonCount);
	}
	for (i = 0; i < chain.size(); ++i)
		result.push_back(chain[i].value);
	return result;
}

std::vector<int> PmergeMe::sortVector(const std::vector<int>& input) const
{
	return mergeInsertSortVector(input);
}

std::deque<int> PmergeMe::sortDeque(const std::deque<int>& input) const
{
	return mergeInsertSortDeque(input);
}

void PmergeMe::printBefore() const
{
	std::size_t i;

	std::cout << "Before:";
	for (i = 0; i < _vectorInput.size(); ++i)
		std::cout << " " << _vectorInput[i];
	std::cout << std::endl;
}

void PmergeMe::printAfter(const std::vector<int>& sorted) const
{
	std::size_t i;

	std::cout << "After:";
	for (i = 0; i < sorted.size(); ++i)
		std::cout << " " << sorted[i];
	std::cout << std::endl;
}

void PmergeMe::run(int argc, char** argv)
{
	parseInput(argc, argv);

	std::vector<int> sortedVector;
	std::deque<int> sortedDeque;
	std::clock_t vectorStart;
	std::clock_t vectorEnd;
	std::clock_t dequeStart;
	std::clock_t dequeEnd;
	double vectorElapsed;
	double dequeElapsed;

	printBefore();
	_vectorComparisonCount = 0;
	_dequeComparisonCount = 0;
	vectorStart = std::clock();
	sortedVector = sortVector(_vectorInput);
	vectorEnd = std::clock();
	dequeStart = std::clock();
	sortedDeque = sortDeque(_dequeInput);
	dequeEnd = std::clock();
	(void)sortedDeque;
	printAfter(sortedVector);
	vectorElapsed = static_cast<double>(vectorEnd - vectorStart)
		* 1000000.0 / static_cast<double>(CLOCKS_PER_SEC);
	dequeElapsed = static_cast<double>(dequeEnd - dequeStart)
		* 1000000.0 / static_cast<double>(CLOCKS_PER_SEC);
	std::cout << std::fixed << std::setprecision(5);
	std::cout << "Time to process a range of " << _vectorInput.size()
		<< " elements with std::vector : " << vectorElapsed << " us" << std::endl;
	std::cout << "Time to process a range of " << _dequeInput.size()
		<< " elements with std::deque : " << dequeElapsed << " us" << std::endl;
	std::cout << "Comparisons with std::vector : "
		<< _vectorComparisonCount << std::endl;
	std::cout << "Comparisons with std::deque : "
		<< _dequeComparisonCount << std::endl;
}
