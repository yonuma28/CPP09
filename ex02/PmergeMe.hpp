#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <ctime>
#include <deque>
#include <string>
#include <vector>

/**
 * @brief Ford-Johnson algorithm の流れで整数列を整列するクラス。
 *
 * コマンドライン引数から正整数列を読み取り、`std::vector<int>` と
 * `std::deque<int>` の 2 種類のコンテナに対して merge-insert sort を実行する。
 */
class PmergeMe
{
	public:
		/**
		 * @brief デフォルトコンストラクタ。
		 */
		PmergeMe();

		/**
		 * @brief コピーコンストラクタ。
		 * @param other コピー元オブジェクト。
		 */
		PmergeMe(const PmergeMe& other);

		/**
		 * @brief 代入演算子。
		 * @param other 代入元オブジェクト。
		 * @return 代入後の自身への参照。
		 */
		PmergeMe& operator=(const PmergeMe& other);

		/**
		 * @brief デストラクタ。
		 */
		~PmergeMe();

		/**
		 * @brief 入力解析、整列、表示、時間計測をまとめて実行する。
		 * @param argc コマンドライン引数数。
		 * @param argv コマンドライン引数配列。
		 * @throw std::runtime_error 入力が不正な場合。
		 */
		void run(int argc, char** argv);

	private:
		/**
		 * @brief 入力順を保持する vector コンテナ。
		 */
		std::vector<int> _vectorInput;

		/**
		 * @brief 入力順を保持する deque コンテナ。
		 */
		std::deque<int> _dequeInput;

		/**
		 * @brief vector ソート中の値比較回数。
		 */
		mutable int _vectorComparisonCount;

		/**
		 * @brief deque ソート中の値比較回数。
		 */
		mutable int _dequeComparisonCount;

		/**
		 * @brief コマンドライン引数を解析してコンテナへ格納する。
		 * @param argc コマンドライン引数数。
		 * @param argv コマンドライン引数配列。
		 * @throw std::runtime_error 入力が不正な場合。
		 */
		void parseInput(int argc, char** argv);

		/**
		 * @brief 正整数文字列を int に変換する。
		 * @param s 変換対象文字列。
		 * @return 変換後の整数値。
		 * @throw std::runtime_error 正整数でない、または int 範囲外の場合。
		 */
		static int parsePositiveInt(const std::string& s);

		/**
		 * @brief 文字列が正整数のみで構成されるか判定する。
		 * @param s 判定対象文字列。
		 * @return `1` 以上の 10 進数字列なら true。
		 */
		static bool isValidPositiveIntegerString(const std::string& s);

		/**
		 * @brief vector 入力を merge-insert sort で整列する。
		 * @param input 整列対象の入力列。
		 * @return 整列済みの vector。
		 */
		std::vector<int> sortVector(const std::vector<int>& input) const;

		/**
		 * @brief vector 用の再帰 merge-insert sort 本体。
		 * @param input 整列対象の入力列。
		 * @return 整列済みの vector。
		 */
		std::vector<int> mergeInsertSortVector(const std::vector<int>& input) const;

		/**
		 * @brief vector の main chain へ自前二分探索で 1 要素挿入する。
		 * @param mainChain 挿入先の整列済み列。
		 * @param value 挿入値。
		 */
		void binaryInsertVector(std::vector<int>& mainChain, int value) const;

		/**
		 * @brief deque 入力を merge-insert sort で整列する。
		 * @param input 整列対象の入力列。
		 * @return 整列済みの deque。
		 */
		std::deque<int> sortDeque(const std::deque<int>& input) const;

		/**
		 * @brief deque 用の再帰 merge-insert sort 本体。
		 * @param input 整列対象の入力列。
		 * @return 整列済みの deque。
		 */
		std::deque<int> mergeInsertSortDeque(const std::deque<int>& input) const;

		/**
		 * @brief deque の main chain へ自前二分探索で 1 要素挿入する。
		 * @param mainChain 挿入先の整列済み列。
		 * @param value 挿入値。
		 */
		void binaryInsertDeque(std::deque<int>& mainChain, int value) const;

		/**
		 * @brief 入力前状態を表示する。
		 */
		void printBefore() const;

		/**
		 * @brief 整列後状態を表示する。
		 * @param sorted 整列済みの vector。
		 */
		void printAfter(const std::vector<int>& sorted) const;
};

#endif // PMERGEME_HPP
