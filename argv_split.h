#include <vector>
#include <string>
#include <algorithm>

#define ESCAPE_HEX_CODE 0x5c
#define SGLQUOTE_HEX_CODE 0x27
#define DBLQUOTE_HEX_CODE 0x22
#define SPACE_HEX_CODE 0x20

class argv_split
{
	struct Tokenizer
	{
		char escape_char;
		std::vector<char> _references;

		Tokenizer() : escape_char(ESCAPE_HEX_CODE), _references({}){};
		Tokenizer(std::vector<char> references) : Tokenizer() { _references = references; };

		std::pair<std::string::const_iterator, char> find_next_unescaped(std::string::const_iterator begin, std::string::const_iterator end)
		{
			struct store_t
			{
				std::string::const_iterator it;
				bool escaped;
			};
			for (store_t store{/* .it = */ begin, /* .escaped = */ false}; store.it != end; ++store.it)
			{
				if (store.escaped)
				{
					store.escaped = false;
				}
				else
				{
					if (*store.it == escape_char)
					{
						store.escaped = true;
					}
					else
					{
						auto token = std::find_if(std::begin(_references), std::end(_references), [&](char c)
												  { return c == *store.it; });
						if (token != std::end(_references))
						{
							return std::make_pair(store.it, *token);
						}
					}
				}
			};

			return std::make_pair(end, '\0');
		};
	};

	struct Token : Tokenizer
	{
		std::string::const_iterator _begin;
		std::string::const_iterator _end;
		char _reference;

		// Token() = delete;
		Token(std::vector<char> references) : Tokenizer(references), _reference('\0'){};

		std::string::const_iterator cbegin() const { return _begin; }
		std::string::const_iterator cend() const { return _end; };
		char reference() const { return _reference; };
	};

	struct Match : Token
	{
		Match(
			std::vector<char> targets,
			std::string::const_iterator begin,
			std::string::const_iterator end) : Token(targets)
		{
			auto match = find_next_unescaped(begin, end);
			_begin = std::get<std::string::const_iterator>(match);
			_reference = std::get<char>(match);
			_end = _begin != end ? std::next(_begin) : end;
		};
	};

	struct EnclosedGroup : Token
	{
		EnclosedGroup(
			std::vector<char> delimiters,
			std::string::const_iterator begin,
			std::string::const_iterator end) : Token(delimiters)
		{
			auto group_begin_match = Match(_references, begin, end);
			_begin = group_begin_match.cbegin();
			_end = Match({group_begin_match.reference()}, group_begin_match.cend(), end).cend();
		};
	};

	template <typename T>
	std::vector<T> _tokenize(const std::string &str, std::vector<char> delimiters)
	{
		std::vector<T> tokens;
		auto begin = std::begin(str);
		auto end = std::end(str);

		do
		{
			T token = T{delimiters, begin, end};

			if (token.cbegin() != token.cend())
			{
				tokens.push_back(token);
			}

			begin = token.cend();
		} while (begin != end);

		return tokens;
	};

private:
	bool prependProgname;
	std::string progname;
	std::vector<std::string> arguments;
	std::vector<const char *> argv_arr;

	void _parse(const std::string &cmdline)
	{
		// process:
		// - find groups not to split
		// - find splitting locations
		// - filter splitting locations within groups
		// - split string on remaining locations

		auto groups = _tokenize<EnclosedGroup>(cmdline, {DBLQUOTE_HEX_CODE, SGLQUOTE_HEX_CODE});
		auto splits = _tokenize<Match>(cmdline, {SPACE_HEX_CODE});

		splits.erase(std::remove_if(std::begin(splits), std::end(splits), [&](auto &split)
									{ return std::any_of(std::begin(groups), std::end(groups), [&](auto &group)
														 { return (group.cbegin() <= split.cbegin() && split.cend() <= group.cend()); }); }),
					 std::end(splits));

		std::vector<std::string::const_iterator> argv_limits{std::cbegin(cmdline)};
		std::transform(std::begin(splits), std::end(splits), std::back_inserter(argv_limits), [&](auto &split)
					   { return std::next(split.cbegin()); });

		std::transform(
			std::begin(argv_limits), std::prev(std::end(argv_limits)),
			std::next(std::begin(argv_limits)),
			std::back_inserter(arguments),
			[&](auto begin, auto end)
			{
				return std::string{begin, std::prev(end)};
			});
		arguments.push_back(std::string{argv_limits.back(), std::cend(cmdline)});
		arguments.erase(std::remove(std::begin(arguments), std::end(arguments), ""), std::end(arguments));
	};

public:
	argv_split() : prependProgname(false) {}

	argv_split(std::string progname) : argv_split()
	{
		prependProgname = true;
		this->progname = progname;
	}

	~argv_split() {}

	std::vector<std::string> getArguments()
	{
		return arguments;
	}

	const char **argv()
	{
		return argv_arr.data();
	}

	const char **parse(const std::string &cmdline)
	{
		// setup/cleanup data
		argv_arr.clear();
		arguments.clear();
		if (prependProgname)
			arguments.push_back(progname);

		// parse the string and populate arguments vector
		_parse(cmdline);

		if (arguments.size() != 0)
		{
			// populate argv array
			for (auto &it : arguments)
				argv_arr.push_back(it.c_str());
			// terminate with null as standard argv array
			argv_arr.push_back(NULL);
		}

		return argv();
	}
};