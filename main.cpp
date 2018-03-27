#include "test.h"
#include "primitivehashmap.h"
#include <iostream>

// List of words that hashes to the same index
// Bucketsize 16:
// song and for, 2 and test and never, 4 and give
// Bucketsize 8:
// ol and FIGHT, sc and to, men and ON

TEST_CASE("PrimitiveHashMap") {
	SECTION("Constructor") {
		PrimitiveHashMap<int> sut(512);
		SECTION("upon construction, has a size of 0") {
			REQUIRE(sut.size() == 0);
		}
	}

	SECTION("set") {
		PrimitiveHashMap<int> sut(5);
		SECTION("when there is capacity, success is returned") {
			bool success = sut.set("never", 5);
			REQUIRE(success);
		}
		SECTION("when there is no capacity, false is returned") {
			PrimitiveHashMap<int> fail(0);
			bool success = fail.set("gonna", 5);
			REQUIRE(!success);
		}
		SECTION("when successful, increases size by 1") {
			sut.set("give", 5);
			REQUIRE(sut.size() == 1);
		}
		SECTION("insertion to capacity will make the table full, thus returning false") {
			bool success = true;
			for (int i = 0; i < 5; ++i) {
				success = sut.set(std::to_string(i), 1);
				REQUIRE(success);
			}
			success = sut.set("you",1);
			REQUIRE(!success);
		}
		SECTION("inserting the same key overrides values and keeps size constant") {
			sut.set("test", 1);
			sut.set("test", 2);
			REQUIRE(sut.size() == 1);
		}
		SECTION("inserting keys that causes collision, adds size by 1") {
			sut.set("song", 1);
			sut.set("for", 2);
			REQUIRE(sut.size() == 2);
			sut.set("2", 3);
			sut.set("test", 4);
			REQUIRE(sut.size() == 4);
		}
	}

	SECTION("get") {
		PrimitiveHashMap<int> sut(7);
		SECTION("calling get on empty table returns NULL") {
			int* actual = sut.get("give");
			REQUIRE(actual == NULL);
		}
		SECTION("calling get on inserted items return the correct value") {
			sut.set("me", 1);
			sut.set("a", 2);
			sut.set("shot", 3);
			sut.set("it", 4);
			sut.set("will", 5);
			sut.set("be", 6);
			sut.set("worthwhile", 7);
			REQUIRE(*sut.get("me") == 1);
			REQUIRE(*sut.get("a") == 2);
			REQUIRE(*sut.get("shot") == 3);
			REQUIRE(*sut.get("it") == 4);
			REQUIRE(*sut.get("will") == 5);
			REQUIRE(*sut.get("be") == 6);
			REQUIRE(*sut.get("worthwhile") == 7);
		}
		SECTION("calling get on erased item will return NULL") {
			sut.set("hello", 1);
			sut.set("world", 2);
			sut.erase("hello");
			sut.erase("world");
			REQUIRE(sut.get("hello") == NULL);
			REQUIRE(sut.get("world") == NULL);
		}
		SECTION("calling get on keys with collisions will retrieve correct values") {
			sut.set("song", 1);
			sut.set("for", 2);
			sut.set("2", 3);
			sut.set("test", 4);
			sut.set("never", 5);
			sut.set("4", 6);
			sut.set("give", 7);
			REQUIRE(*sut.get("song") == 1);
			REQUIRE(*sut.get("for") == 2);
			REQUIRE(*sut.get("2") == 3);
			REQUIRE(*sut.get("test") == 4);
			REQUIRE(*sut.get("never") == 5);
			REQUIRE(*sut.get("4") == 6);
			REQUIRE(*sut.get("give") == 7);
		}
	}

	SECTION("erase") {
		PrimitiveHashMap<int> sut(8);
		SECTION("when table is empty, NULL is returned") {
			int* actual = sut.erase("hey");
			REQUIRE(actual == NULL);
		}
		SECTION("erasing after inserting with the same key returns the value") {
			sut.set("up", 5);
			int* actual = sut.erase("up");
			REQUIRE(actual != NULL);
			if (actual != NULL) {
				REQUIRE(*actual == 5);
			}
		}
		SECTION("erasing colliding keys, returns the value deleted and decreases size by 1") {
			sut.set ("song", 1);
			sut.set("for", 2);
			sut.set("2", 3);
			sut.set("test", 4);
			sut.set("never",5);
			int* actual = sut.erase("2");
			REQUIRE(actual != NULL);
			REQUIRE(*actual == 3);
			REQUIRE(sut.size() == 4);
			actual = sut.erase("never");
			REQUIRE(actual != NULL);
			REQUIRE(*actual == 5);
			REQUIRE(sut.size() == 3);
			actual = sut.erase("song");
			REQUIRE(actual != NULL);
			REQUIRE(*actual == 1);
			REQUIRE(sut.size() == 2);
			actual = sut.erase("for");
			REQUIRE(actual != NULL);
			REQUIRE(*actual == 2);
			REQUIRE(sut.size() == 1);
			actual = sut.erase("test");
			REQUIRE(actual != NULL);
			REQUIRE(*actual == 4);
			REQUIRE(sut.size() == 0);
		}
	}

	SECTION("load") {
		PrimitiveHashMap<int> sut(4);
		SECTION("load factor of an empty table is 0.0") {
			float actual = sut.load();
			REQUIRE(actual == 0.0);
		}
		SECTION("load factor of one item inserted with bucket 8 is 1/8") {
			sut.set("ol", 1);
			float expected = static_cast<float>(1)/static_cast<float>(8);
			float actual = sut.load();
			REQUIRE(actual == expected);
		}
		SECTION("load factor of two item inserted with bucket 8 is 2/8") {
			sut.set("sc", 1);
			sut.set("our", 2);
			float expected = static_cast<float>(2)/static_cast<float>(8);
			float actual = sut.load();
			REQUIRE(actual == expected);
		}
		SECTION("load factor of three item inserted with bucket 8 is 3/8") {
			sut.set("men", 1);
			sut.set("FIGHT", 2);
			sut.set("ON", 3);
			float expected = static_cast<float>(3)/static_cast<float>(8);
			float actual = sut.load();
			REQUIRE(actual == expected);
		}
		SECTION("load factor of four item inserted with bucket 8 is 4/8") {
			sut.set("to", 1);
			sut.set("victory", 2);
			sut.set("!", 3);
			sut.set("!!", 4);
			float expected = static_cast<float>(4)/static_cast<float>(8);
			float actual = sut.load();
			REQUIRE(actual == expected);
		}
	}
}

int main(int argc, char** argv) {
	int result = Catch::Session().run(argc, argv);
	return result;
}
