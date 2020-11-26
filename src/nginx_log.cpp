#include <nginx_log.h>
#include <unordered_map>
#include <map>
#include <fstream>
#include <filesystem>
#include <iterator>
#include <vlq.h>
#include <sstream>
#include <vector>
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

    const std::filesystem::path terms_path;
    std::unordered_map<std::string, uint32_t> terms_to_ids;

    explicit preprocessed_log(const std::string &log_filename) : terms_path(
      std::filesystem::path(log_filename).replace_extension(".terms")
    ) {
      std::ifstream log(log_filename);
      // В один проход по потоку логов заполняем файл термированными url
      std::ofstream termed_urls(terms_path);
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
    }

    virtual ~preprocessed_log() {
      // std::filesystem::remove(terms_filename);
    }
  };
}

void nginx_log::compress(const std::string &log_filename) {
  preprocessed_log log(log_filename);
  std::ifstream preprocessed_stream(log.terms_path);
  std::ofstream compressed_stream(
    std::filesystem::path(log_filename).replace_extension(".v2"),
    std::ios::binary
  );
  compressed_stream << log.terms_to_ids.size();
  for (auto &&[term, _]: log.terms_to_ids) {
    compressed_stream << ' ' << term;
  }
  for (auto &&[_, id]: log.terms_to_ids) {
    compressed_stream << vlq::from(id);
  }
  for (std::string line; std::getline(preprocessed_stream, line);) {
    auto terms = split(line);
    compressed_stream << vlq::from(terms.size());
    for (auto &&term: terms) {
      compressed_stream << vlq::from(log.terms_to_ids[term]);
    }
  }
}

void nginx_log::decompress(const std::string &v2_filename) {

  std::ifstream vlq_compressed_stream(v2_filename, std::ios::binary);

  uint32_t ids_to_terms_size;
  vlq_compressed_stream >> ids_to_terms_size;

  std::vector<std::string> ordered_terms(ids_to_terms_size);
  for (auto &term: ordered_terms) {
    vlq_compressed_stream >> term;
  }

  std::vector<vlq::number> ordered_ids(ids_to_terms_size);
  for (auto &id: ordered_ids) {
    vlq_compressed_stream >> id;
  }

  std::unordered_map<uint32_t, std::string> ids_to_terms(ids_to_terms_size);
  for (uint32_t i = 0; i < ids_to_terms_size; ++i) {
    ids_to_terms.emplace(vlq::to_uint(ordered_ids[i]), std::move(ordered_terms[i]));
  }

  auto decompressed_path = std::filesystem::path(v2_filename).replace_extension(".urls");
  std::ofstream decompressed_stream(decompressed_path);
  vlq::number url_words_count, id_buf;

  while (vlq_compressed_stream) {

    vlq_compressed_stream >> url_words_count;
    for (uint32_t i = 0; i < vlq::to_uint(url_words_count); ++i) {
      vlq_compressed_stream >> id_buf;
      decompressed_stream << ids_to_terms[vlq::to_uint(id_buf)] << ' ';
    }
    decompressed_stream << '\n';
  }
}
