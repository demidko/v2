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

  struct preprocessed_log {

    const std::filesystem::path terms_path;
    std::unordered_map<std::string, uint32_t> terms_to_ids;
    uint32_t lines_count = 0;

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
        ++lines_count;
      }
      // Сортируем термы по возрастанию частоты
      std::multimap<uint32_t, std::reference_wrapper<const std::basic_string<char>>> frequency_map;
      for (auto &&[term, frequency]: terms_to_ids) {
        frequency_map.emplace(frequency, std::ref(term));
      }
      // Заменяем частоту на идентификаторы в несортированном словаре термов
      auto identifier = std::size(terms_to_ids) + 1u;
      for (auto &&[_, term]: frequency_map) {
        terms_to_ids[term] = --identifier;
      }
    }

    virtual ~preprocessed_log() { /*std::filesystem::remove(terms_path);*/ }
  };
}

#include <iostream>

void nginx_log::compress(const std::string &log_filename) {
  preprocessed_log log(log_filename);
  std::ifstream preprocessed_stream(log.terms_path);
  std::ofstream compressed_stream(
    std::filesystem::path(log_filename).replace_extension(".v2"),
    std::ios::binary
  );
  compressed_stream << log.terms_to_ids.size() << ' ';
  for (auto &&[term, _]: log.terms_to_ids) {
    compressed_stream << term << ' ';
  }
  for (auto &&[_, id]: log.terms_to_ids) {
    compressed_stream << vlq::wrap(id);
  }

  std::cout << "COMPRESSED STREAM\n";
  for (auto &&[k, v]: log.terms_to_ids) {
    std::cout << v << ' ' << k << std::endl;
  }
  std::cout << "COMPRESSED LINES " << log.lines_count << std::endl;

  compressed_stream << vlq::wrap(log.lines_count);
  for (std::string buf; std::getline(preprocessed_stream, buf);) {
    for (auto &words = parse(buf); std::getline(words, buf, ' ');) {
      std::cout << log.terms_to_ids[buf] << ' ';
      compressed_stream << vlq::wrap(log.terms_to_ids[buf]);
    }
    std::cout << 0 << ' ';
    compressed_stream << vlq::wrap(0);
  }
  compressed_stream << std::flush;
}


void nginx_log::decompress(const std::string &v2_filename) {

  std::ifstream vlq_compressed_stream(v2_filename, std::ios::binary);

  uint32_t ids_to_terms_size;
  vlq_compressed_stream >> ids_to_terms_size;

  std::vector<std::string> ordered_terms(ids_to_terms_size);
  for (auto &term: ordered_terms) {
    vlq_compressed_stream >> term;
  }

  vlq_compressed_stream.ignore();

  std::vector<vlq::number> ordered_ids(ids_to_terms_size);
  for (auto &id: ordered_ids) {
    vlq_compressed_stream >> id;
  }

  std::unordered_map<uint32_t, std::string> ids_to_terms(ids_to_terms_size);
  for (uint32_t i = 0; i < ids_to_terms_size; ++i) {
    ids_to_terms.emplace(vlq::unwrap(ordered_ids[i]), std::move(ordered_terms[i]));
  }


  std::ofstream decompressed_stream(std::filesystem::path(v2_filename).replace_extension(".urls"));
  vlq::number lines_count;
  vlq_compressed_stream >> lines_count;


  std::cout << "\n\nDECOMPRESSED STREAM\n";
  for (auto &&[k, v]: ids_to_terms) {
    std::cout << k << ' ' << v << std::endl;
  }
  std::cout << "DECOMPRESSED LINES " << vlq::unwrap(lines_count) << std::endl;


  for (uint32_t i = 0, limit = vlq::unwrap(lines_count); i < limit; ++i) {

    for (vlq::number buffer; vlq_compressed_stream >> buffer;) {
      if (auto id = vlq::unwrap(buffer); id) {
        std::cout << id << ' ';
        decompressed_stream << ids_to_terms[id] << ' ';
      } else {
        std::cout << 0 << ' ';
        decompressed_stream << '\n';
        break;
      }
    }

  }
}
