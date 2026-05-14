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

	void insertVectorNodeByBound(std::vector<VectorNode>& chain, const VectorNode& node, std::size_t high)
	{
		std::size_t low = 0;

		while (low < high)
		{
			std::size_t mid = low + (high - low) / 2;

			if (chain[mid].value < node.value)
				low = mid + 1;
			else
				high = mid;
		}
		chain.insert(chain.begin() + static_cast<std::vector<VectorNode>::difference_type>(low), node);
	}

	void insertDequeNodeByBound(std::deque<DequeNode>& chain, const DequeNode& node, std::size_t high)
	{
		std::size_t low = 0;

		while (low < high)
		{
			std::size_t mid = low + (high - low) / 2;

			if (chain[mid].value < node.value)
				low = mid + 1;
			else
				high = mid;
		}
		chain.insert(chain.begin() + static_cast<std::deque<DequeNode>::difference_type>(low), node);
	}

	void insertVectorPairByBound(std::vector<VectorPair>& chain, const VectorPair& pair, std::size_t high)
	{
		std::size_t low = 0;

		while (low < high)
		{
			std::size_t mid = low + (high - low) / 2;

			if (chain[mid].large < pair.large)
				low = mid + 1;
			else
				high = mid;
		}
		chain.insert(chain.begin() + static_cast<std::vector<VectorPair>::difference_type>(low), pair);
	}

	void insertDequePairByBound(std::deque<DequePair>& chain, const DequePair& pair, std::size_t high)
	{
		std::size_t low = 0;

		while (low < high)
		{
			std::size_t mid = low + (high - low) / 2;

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

	std::vector<VectorPair> sortVectorPairsByLarge(const std::vector<VectorPair>& input)
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
		sortedLargePairs = sortVectorPairsByLarge(largePairs);
		mainChain.push_back(findVectorPendingByPartnerId(pendingPairs, sortedLargePairs[0].id));
		for (i = 0; i < sortedLargePairs.size(); ++i)
			mainChain.push_back(sortedLargePairs[i]);
		insertionOrder = buildJacobsthalOrderVector(sortedLargePairs.size());
		for (i = 0; i < insertionOrder.size(); ++i)
		{
			std::size_t partnerIndex = insertionOrder[i];
			std::size_t partnerPos;
			VectorPair partnerSmall = findVectorPendingByPartnerId(pendingPairs, sortedLargePairs[partnerIndex].id);

			for (partnerPos = 0; partnerPos < mainChain.size(); ++partnerPos)
			{
				if (mainChain[partnerPos].id == sortedLargePairs[partnerIndex].id)
					break;
			}
			insertVectorPairByBound(mainChain, partnerSmall, partnerPos);
		}
		if (hasLeftover)
			insertVectorPairByBound(mainChain, leftover, mainChain.size());
		return mainChain;
	}

	std::deque<DequePair> sortDequePairsByLarge(const std::deque<DequePair>& input)
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
		sortedLargePairs = sortDequePairsByLarge(largePairs);
		mainChain.push_back(findDequePendingByPartnerId(pendingPairs, sortedLargePairs[0].id));
		for (i = 0; i < sortedLargePairs.size(); ++i)
			mainChain.push_back(sortedLargePairs[i]);
		insertionOrder = buildJacobsthalOrderDeque(sortedLargePairs.size());
		for (i = 0; i < insertionOrder.size(); ++i)
		{
			std::size_t partnerIndex = insertionOrder[i];
			std::size_t partnerPos;
			DequePair partnerSmall = findDequePendingByPartnerId(pendingPairs, sortedLargePairs[partnerIndex].id);

			for (partnerPos = 0; partnerPos < mainChain.size(); ++partnerPos)
			{
				if (mainChain[partnerPos].id == sortedLargePairs[partnerIndex].id)
					break;
			}
			insertDequePairByBound(mainChain, partnerSmall, partnerPos);
		}
		if (hasLeftover)
			insertDequePairByBound(mainChain, leftover, mainChain.size());
		return mainChain;
	}
}

PmergeMe::PmergeMe()
{
}

PmergeMe::PmergeMe(const PmergeMe& other)
	: _vectorInput(other._vectorInput), _dequeInput(other._dequeInput)
{
}

PmergeMe& PmergeMe::operator=(const PmergeMe& other)
{
	if (this != &other)
	{
		_vectorInput = other._vectorInput;
		_dequeInput = other._dequeInput;
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
	sortedPairs = sortVectorPairsByLarge(pairs);
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
	insertionOrder = buildJacobsthalOrderVector(sortedPairs.size());
	for (i = 0; i < insertionOrder.size(); ++i)
	{
		std::size_t pairIndex = insertionOrder[i];
		std::size_t partnerPos = findVectorLargePosition(chain, sortedPairs[pairIndex].id);
		VectorNode node;

		node.value = sortedPairs[pairIndex].small;
		node.pairId = sortedPairs[pairIndex].id;
		node.isLarge = false;
		insertVectorNodeByBound(chain, node, partnerPos);
	}
	result.reserve(chain.size() + (hasLeftover ? 1 : 0));
	for (i = 0; i < chain.size(); ++i)
		result.push_back(chain[i].value);
	if (hasLeftover)
		binaryInsertVector(result, leftover);
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
	sortedPairs = sortDequePairsByLarge(pairs);
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
	insertionOrder = buildJacobsthalOrderDeque(sortedPairs.size());
	for (i = 0; i < insertionOrder.size(); ++i)
	{
		std::size_t pairIndex = insertionOrder[i];
		std::size_t partnerPos = findDequeLargePosition(chain, sortedPairs[pairIndex].id);
		DequeNode node;

		node.value = sortedPairs[pairIndex].small;
		node.pairId = sortedPairs[pairIndex].id;
		node.isLarge = false;
		insertDequeNodeByBound(chain, node, partnerPos);
	}
	for (i = 0; i < chain.size(); ++i)
		result.push_back(chain[i].value);
	if (hasLeftover)
		binaryInsertDeque(result, leftover);
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
}
