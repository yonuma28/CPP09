#ifndef RPN_HPP
#define RPN_HPP

#include <stack>
#include <string>

/**
 * @brief Reverse Polish Notation の式を評価するクラス。
 *
 * 空白区切りの RPN 式を読み、`0` から `9` の 1 桁整数と
 * `+`, `-`, `*`, `/` を使った計算結果を返す。
 */
class RPN
{
	public:
		/**
		 * @brief デフォルトコンストラクタ。
		 */
		RPN();

		/**
		 * @brief コピーコンストラクタ。
		 * @param other コピー元オブジェクト。
		 */
		RPN(const RPN& other);

		/**
		 * @brief 代入演算子。
		 * @param other 代入元オブジェクト。
		 * @return 代入後の自身への参照。
		 */
		RPN& operator=(const RPN& other);

		/**
		 * @brief デストラクタ。
		 */
		~RPN();

		/**
		 * @brief RPN 式を評価して結果を返す。
		 * @param expression 空白区切りの RPN 式。
		 * @return 評価結果。
		 * @throw std::runtime_error 式が不正な場合。
		 */
		long evaluate(const std::string& expression) const;

	private:
		/**
		 * @brief token が演算子か判定する。
		 * @param token 判定対象の token。
		 * @return `+`, `-`, `*`, `/` のいずれかなら true。
		 */
		bool isOperator(const std::string& token);

		/**
		 * @brief token が 1 桁の数値か判定する。
		 * @param token 判定対象の token。
		 * @return `0` から `9` の 1 文字なら true。
		 */
		bool isNumberToken(const std::string& token);

		/**
		 * @brief 2 つの値に演算子を適用する。
		 * @param lhs 左辺値。
		 * @param rhs 右辺値。
		 * @param op 演算子 token。
		 * @return 演算結果。
		 * @throw std::runtime_error 不正な演算子または 0 除算の場合。
		 */
		long applyOperator(long lhs, long rhs, const std::string& op);

		long checkedAdd(long lhs, long rhs);
		long checkedSubtract(long lhs, long rhs);
		long checkedMultiply(long lhs, long rhs);
		long checkedDivide(long lhs, long rhs);
};

#endif // RPN_HPP
