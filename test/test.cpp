#define CATCH_CONFIG_MAIN

#include <catch.hpp>
#include <vlq.h>
#include <iostream>
#include <fstream>
#include <bit.h>
#include <filesystem>
#include <list>
#include <nginx_log.h>

TEST_CASE("Bit operations should works correctly") {
  uint64_t expected = 475'187'001;
  uint64_t actual{};
  for (uint8_t i = 0; i < 64; ++i) {
    if (bit::get(expected, i)) {
      bit::set(actual, i);
    }
  }
  REQUIRE(actual == expected);
}

TEST_CASE("Standard operators '>>' and '<<' should works correctly with binary IO") {
  {
    std::ofstream bin_out("test.bin", std::ios::binary);
    bin_out << 47502317u << " some text and more " << vlq::wrap(4) << vlq::wrap(0) << vlq::wrap(475);
  }
  std::ifstream bin_in("test.bin", std::ios::binary);
  uint64_t len;
  vlq::number vlq1, vlq2, vlq3;
  std::string some, text, and_, more;
  bin_in >> len >> some >> text >> and_ >> more;
  bin_in.ignore();
  bin_in >> vlq1 >> vlq2 >> vlq3;
  std::filesystem::remove("test.bin");
  REQUIRE(len == 47502317u);
  REQUIRE(some == "some");
  REQUIRE(text == "text");
  REQUIRE(and_ == "and");
  REQUIRE(more == "more");
  REQUIRE(vlq::unwrap(vlq1) == 4);
  REQUIRE(vlq::unwrap(vlq2) == 0);
  REQUIRE(vlq::unwrap(vlq3) == 475);
}

TEST_CASE("Vlq compression should works correctly in 0..4'294'967'295 range") {

  // Сюда уходит основное время тестов:
  // проверяем vlq на списке из 10 миллионов случайных чисел
  constexpr auto test_list_size = 10'000'000;

  auto generate_random_list = [] {
    std::list<uint32_t> list;
    std::generate_n(
      std::back_inserter(list),
      test_list_size,
      std::bind(
        std::uniform_int_distribution<uint32_t>(0, UINT32_MAX),
        std::mt19937{std::random_device{}()}
      )
    );
    return list;
  };

  std::list<uint32_t> expected_list = generate_random_list();
  {
    std::ofstream bin_out("test.bin", std::ios::binary);
    for (auto &&x: expected_list) {
      bin_out << vlq::wrap(x);
    }
  }

  std::ifstream bin_in("test.bin", std::ios::binary);
  std::list<uint32_t> actual_list;
  vlq::number buf;
  for (auto i = 0; i < test_list_size; ++i) {
    bin_in >> buf;
    actual_list.push_back(vlq::unwrap(buf));
  }
  std::filesystem::remove("test.bin");

  REQUIRE(actual_list == expected_list);
}

TEST_CASE("Complex utility test") {
  auto log_text = "2019-12-25T00:01:02.176211+10:00 baza.farpost.ru_log: \"www.farpost.ru\" 188.162.229.90 - - [25/Dec/2019:00:01:02 +1000] \"GET /vladivostok/children/clothes-boots/clothes/+/%CA%E8%E3%F3%F0%F3%EC%E8/?fitTo%5B%5D=4&query=%CA%E8%E3%F3%F0%F3%EC%E8%2C+%C4%E5%F2%F1%EA%E8%E5 HTTP/1.1\" \"200\" \"59842\" \"1019\" \"0.127\" 192.168.36.21 \"0.126\" \"200\" \"\" \"4d9148318786de0c21d4cf929d05b18b\" \"0.000\" \"0\" \"viewDirController::view\" \"https://www.farpost.ru/vladivostok/children/clothes-boots/clothes/+/%CA%E8%E3%F3%F0%F3%EC%E8/?fitTo%5B%5D=4&query=%CA%E8%E3%F3%F0%F3%EC%E8%2C+%C4%E5%F2%F1%EA%E8%E5\" \"Mozilla/5.0 (Linux; Android 8.1.0; JSN-L22 Build/HONORJSN-L22) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.91 Mobile Safari/537.36\" \"on\" \"-\" \"-\" \"4d9148318786de0c21d4cf929d05b18b\" \"\" \"\" \"192.168.36.128\" \"80.92.164.135\" \"7.3.10\" \"-\" \"unix:/tmp/php7.3.10-fpm.sock\"\n"
                  "2019-12-25T00:01:02.176301+10:00 baza.farpost.ru_log: \"baza.farpost.ru\" 185.44.0.50 - - [25/Dec/2019:00:01:02 +1000] \"POST /ws/identification HTTP/1.1\" \"200\" \"813\" \"667\" \"0.015\" 192.168.36.16 \"0.014\" \"200\" \"\" \"-\" \"0.000\" \"0\" \"identificationWsControllerOld::run\" \"-\" \"DROP/CURL-SOAP\" \"\" \"-\" \"-\" \"e785cc35af36c6efaff90774c9d754bf\" \"\" \"\" \"192.168.36.128\" \"80.92.164.136\" \"7.3.10\" \"-\" \"unix:/tmp/php7.3.10-fpm-wsident.sock\"\n"
                  "2019-12-25T00:01:02.176560+10:00 baza.farpost.ru_log: \"my.drom.ru\" 92.124.163.153 - - [25/Dec/2019:00:01:02 +1000] \"POST /api/1.0/bookmark/add-batch?boobs=490e4821d93b0bde7a5a614312a2d5c47f47ef4f2dbff9a3e07922d9c2f7aa17ud28ed3&objectType=dromBulletin&recSysDeviceId=ad6d9ddbb89a8cc6a5ec44c7f2f87989&recSysRegionId=55&recSysCityId=113 HTTP/1.1\" \"200\" \"126\" \"1634\" \"0.051\" 192.168.36.17 \"0.051\" \"200\" \"0xd28ed3\" \"aeada33XPJPMyQ6S5aNqkfAs%2F17SQ0a7\" \"0.000\" \"0\" \"FarPost/Baza/Favorites/Api/FavoritesApiController::addFavoritesBatch\" \"-\" \"DromAuto/3.11.2 (Android; samsung; SM-A307FN; 1.75)\" \"on\" \"-\" \"-\" \"aeada33XPJPMyQ6S5aNqkfAs/17SQ0a7\" \"\" \"\" \"192.168.36.128\" \"80.92.164.153\" \"7.3.10\" \"-\" \"unix:/tmp/php7.3.10-fpm.sock\"\n"
                  "2019-12-25T00:01:02.176948+10:00 baza.farpost.ru_log: \"my.drom.ru\" 178.184.150.221 - - [25/Dec/2019:00:01:02 +1000] \"GET /personal/messaging/last-seen?login=15812551&ajax=1 HTTP/1.1\" \"200\" \"48\" \"1603\" \"0.016\" 192.168.36.26 \"0.017\" \"200\" \"0x1a6595\" \"67920bdJwqVsWWK%2B3OutZyqqEcfAw0a8\" \"0.000\" \"0\" \"FarPost/Baza/Messaging/Controller/UserDetailController::lastSeen\" \"https://my.drom.ru/personal/messaging-modal/dialog-606838105\" \"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36\" \"on\" \"-\" \"-\" \"67920bdJwqVsWWK+3OutZyqqEcfAw0a8\" \"\" \"\" \"192.168.36.129\" \"80.92.164.156\" \"7.3.10\" \"-\" \"unix:/tmp/php7.3.10-fpm.sock\"\n"
                  "2019-12-25T00:01:02.177082+10:00 baza.farpost.ru_log: \"www.farpost.ru\" 31.173.240.202 - - [25/Dec/2019:00:01:02 +1000] \"GET /backend/remarketing-api/api/v1.0/interests?ring=24871e4l5S1QDTSYAhWEe9ETuIGYQ0a0 HTTP/1.1\" \"200\" \"90\" \"594\" \"0.004\" 192.168.36.20 \"0.004\" \"200\" \"\" \"-\" \"0.000\" \"-\" \"-\" \"https://www.drom.ru/\" \"Mozilla/5.0 (Linux; Android 8.0.0; F8332) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.96 Mobile Safari/537.36\" \"on\" \"-\" \"-\" \"-\" \"\" \"\" \"192.168.36.128\" \"80.92.164.136\" \"7.3.10\" \"upstream_backend_remarketing_api\" \"192.168.33.253:8103\"";
  std::ofstream("test.log") << log_text;
  nginx_log::compress("test.log");
  nginx_log::decompress("test.v2");
}

