#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <map>
#include <string>

/**
 * @brief Bitcoin の為替データベースを読み込み、入力ファイルを評価するクラス。
 *
 * `data.csv` から日付ごとのレートを読み込み、
 * `date | value` 形式の入力に対して `value * rate` を出力する。
 */
class BitcoinExchange
{
	private:
		std::map<std::string, double> _rates;

	public:
		/**
		 * @brief 空の BitcoinExchange を生成する。
		 */
		BitcoinExchange(void);

		/**
		 * @brief データベースを読み込んだ状態で BitcoinExchange を生成する。
		 * @param dbPath 読み込むデータベースファイルのパス。
		 */
		BitcoinExchange(const std::string& dbPath);

		/**
		 * @brief コピーコンストラクタ。
		 * @param other コピー元オブジェクト。
		 */
		BitcoinExchange(const BitcoinExchange& other);

		/**
		 * @brief 代入演算子。
		 * @param other 代入元オブジェクト。
		 * @return 代入後の自身への参照。
		 */
		BitcoinExchange& operator=(const BitcoinExchange& other);

		/**
		 * @brief デストラクタ。
		 */
		~BitcoinExchange(void);

		/**
		 * @brief DB ファイルを読み込み、内部レート表を初期化する。
		 * @param dbPath 読み込むデータベースファイルのパス。
		 * @throw std::runtime_error ファイルを開けない、または有効なデータがない場合。
		 */
		void loadDatabase(const std::string& dbPath);

		/**
		 * @brief 入力ファイルを 1 行ずつ評価して結果またはエラーを出力する。
		 * @param inputPath 評価対象の入力ファイルのパス。
		 * @throw std::runtime_error ファイルを開けない場合。
		 */
		void processInputFile(const std::string& inputPath) const;

	private:
		/**
		 * @brief 文字列の前後の空白文字を削除する。
		 * @param s 整形対象の文字列。
		 * @return 前後の空白を除去した文字列。
		 */
		static std::string trim(const std::string& s);

		/**
		 * @brief 日付文字列が厳密な `YYYY-MM-DD` 形式で妥当か検証する。
		 * @param date 検証する日付文字列。
		 * @return 妥当なら true、そうでなければ false。
		 */
		static bool isValidDate(const std::string& date);

		/**
		 * @brief 文字列を double として厳密に読み取る。
		 * @param s 解析対象の文字列。
		 * @param out 読み取り成功時に値を書き込む出力先。
		 * @return 数値として正しく解析できた場合 true、失敗した場合 false。
		 */
		static bool parsePositiveNumber(const std::string& s, double& out);

		/**
		 * @brief DB の 1 行を解析して日付とレートを取り出す。
		 * @param line 解析対象の 1 行。
		 * @param date 解析成功時に日付を書き込む出力先。
		 * @param rate 解析成功時にレートを書き込む出力先。
		 * @return 行が有効なら true、無効なら false。
		 */
		static bool parseDatabaseLine(const std::string& line, std::string& date, double& rate);

		/**
		 * @brief 入力行を `date | value` の 2 項目に分解する。
		 * @param line 解析対象の 1 行。
		 * @param date 解析成功時に日付を書き込む出力先。
		 * @param valueField 解析成功時に値部分の文字列を書き込む出力先。
		 * @return 区切り文字 `|` を含む場合 true、そうでなければ false。
		 */
		static bool splitInputLine(const std::string& line, std::string& date, std::string& valueField);

		/**
		 * @brief 入力値が先頭 `-` を持つ負数表記か判定する。
		 * @param valueField 入力行から取り出した値部分。
		 * @return 負数表記なら true、そうでなければ false。
		 */
		static bool isNegativeValue(const std::string& valueField);

		/**
		 * @brief 入力ファイルの 1 行を評価して結果またはエラーを出力する。
		 * @param line 評価対象の入力行。
		 */
		void processInputLine(const std::string& line) const;

		/**
		 * @brief 指定日以下で最も近いレートを検索する。
		 * @param date 検索基準の日付。
		 * @param rate 検索成功時にレートを書き込む出力先。
		 * @return 対応するレートが見つかった場合 true、見つからない場合 false。
		 */
		bool findRateForDate(const std::string& date, double& rate) const;

		/**
		 * @brief 指定年がうるう年か判定する。
		 * @param year 判定対象の年。
		 * @return うるう年なら true、そうでなければ false。
		 */
		static bool isLeapYear(int year);

		/**
		 * @brief 指定した年月の日数を返す。
		 * @param year 判定対象の年。
		 * @param month 判定対象の月。
		 * @return その月の日数。
		 */
		static int daysInMonth(int year, int month);

		/**
		 * @brief DB 側の日付表記を `YYYY-MM-DD` に正規化する。
		 * @param rawDate DB から読み取った元の日付文字列。
		 * @return 正規化後の日付文字列。正規化できない場合は元の文字列に近い形で返す。
		 */
		static std::string normalizeDatabaseDate(const std::string& rawDate);
};

#endif //BITCOINEXCHANGE_HPP
