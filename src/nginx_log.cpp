#include <nginx_log.h>
#include <unordered_map>
#include <map>
#include <fstream>
#include <filesystem>
#include <iterator>
#include <vlq.h>
#include <sstream>
#include <list>

namespace {

  std::istringstream &parse(const std::string &text) {
    static std::istringstream parser;
    parser.clear();
    parser.str(text);
    return parser;
  }

  std::string extract(const std::string &from_text, int word_at) {
    return *std::next(std::istream_iterator<std::string>(parse(from_text)), word_at);
  }

  std::list<std::string> split(const std::string &text) {
    auto &line = parse(text);
    std::list<std::string> result;
    for (std::string word; std::getline(line, word, ' ');) {
      result.push_back(word);
    }
    return result;
  }

  struct preprocessed_log {

    const std::string terms_filename;
    std::unordered_map<std::string, uint32_t> terms_to_ids;

    explicit preprocessed_log(const std::string &log_filename) : terms_filename(log_filename + ".terms") {
      std::ifstream log(log_filename);
      // В один проход по потоку логов заполняем файл термированными url
      std::ofstream termed_urls(terms_filename);
      // И генерируем словарь термов с частотой
      for (std::string buf; std::getline(log, buf);) {
        auto request = extract(buf, 23);
        auto first = request.find("//");
        auto last = request.find_first_of("?\"", first);
        if (first == -1 || last == -1) {
          // значит это не url, а что нибудь вроде "-" или "5aef8db%2FT0w16Io%2FmYOshHdtzlxGA0ab"
          // (таких значений примерно треть на 100 000 записей)
          continue;
        }
        auto url = request.substr(first + 2, last - first - 2);
        for (auto &terms = parse(url); std::getline(terms, buf, '/');) {
          if (!buf.empty()) {
            ++terms_to_ids[buf];
            termed_urls << buf << ' ';
          }
        }
        termed_urls << '\n';
      }
      // Сортируем термы по возрастанию частоты
      std::multimap<uintmax_t, std::reference_wrapper<const std::basic_string<char>>> frequency_map;
      for (auto &&[term, frequency]: terms_to_ids) {
        frequency_map.emplace(frequency, std::ref(term));
      }
      // Заменяем частоту на идентификаторы в несортированном словаре термов
      auto identifier = std::size(terms_to_ids) + 1u;
      for (auto &&[_, term]: frequency_map) {
        terms_to_ids[term] = --identifier;
      }
      termed_urls.close();
      std::ifstream a;
      std::ifstream b(std::move(a));
    }

    virtual ~preprocessed_log() {
      // std::filesystem::remove(terms_filename);
    }
  };
}

void nginx_log::compress(const std::string &log_filename) {
  preprocessed_log log(log_filename);
  std::ifstream preprocessed_stream(log.terms_filename);
  std::ofstream compressed_stream(log_filename + ".v2", std::ios::binary);
  compressed_stream << log.terms_to_ids.size();
  for (auto &&[term, _]: log.terms_to_ids) {
    compressed_stream << term << ' ';
  }
  for (auto &&[_, id]: log.terms_to_ids) {
    compressed_stream << vlq::as_vlq(id);
  }
  for (std::string line; std::getline(preprocessed_stream, line);) {
    auto terms = split(line);
    compressed_stream << vlq::as_vlq(terms.size());
    for (auto &&term: terms) {
      compressed_stream << vlq::as_vlq(log.terms_to_ids[term]);
    }
  }
}

void nginx_log::decompress(const std::string &v2_filename) {
  std::ifstream vlq_binary_stream(v2_filename, std::ios::binary);
  std::list<std::string> term;
  std::size_t terms_to_ids_size;
  vlq_binary_stream.read(reinterpret_cast<char *>(&terms_to_ids_size), sizeof(terms_to_ids_size));
}
