#pragma once

#include <string>
#include <fc/reflect/reflect.hpp>
#include <iosfwd>

namespace eosio {
   namespace chain {
      using std::string;

      #define AN(X) eosio::chain::account::string_to_name(#X)


         struct account_name {
         private:
            uint64_t value = 0;

         public:
            bool empty() const { return 0 == value; }

            bool good() const { return !empty(); }

            account_name(const char *str) { set(str); }

            account_name(const string &str) { set(str.c_str()); }

            void set(const char *str);

            template<typename T>
            account_name(T v):value(v) {}

            account_name() {}

            uint64_t get_value() const {
               return this->value;
            }

            static constexpr uint64_t char_to_symbol(char c) {
               if (c >= 'a' && c <= 'z')
                  return (c - 'a') + 6;
               if (c >= '1' && c <= '5')
                  return (c - '1') + 1;
               return 0;
            }

            // Each char of the string is encoded into 5-bit chunk and left-shifted
            // to its 5-bit slot starting with the highest slot for the first char.
            // The 13th char, if str is long enough, is encoded into 4-bit chunk
            // and placed in the lowest 4 bits. 64 = 12 * 5 + 4
            static constexpr uint64_t string_to_name(const char *str) {
               uint64_t name = 0;
               int i = 0;
               for (; str[i] && i < 12; ++i) {
                  // NOTE: char_to_symbol() returns char type, and without this explicit
                  // expansion to uint64 type, the compilation fails at the point of usage
                  // of string_to_name(), where the usage requires constant (compile time) expression.
                  name |= (char_to_symbol(str[i]) & 0x1f) << (64 - 5 * (i + 1));
               }

               // The for-loop encoded up to 60 high bits into uint64 'name' variable,
               // if (strlen(str) > 12) then encode str[12] into the low (remaining)
               // 4 bits of 'name'
               if (i == 12)
                  name |= char_to_symbol(str[12]) & 0x0F;
               return name;
            }

            explicit operator string() const;

            string to_string() const { return string(*this); }

            account_name &operator=(uint64_t v) {
               value = v;
               return *this;
            }

            account_name &operator=(const string &n) {
               value = account_name(n).value;
               return *this;
            }

            account_name &operator=(const char *n) {
               value = account_name(n).value;
               return *this;
            }

            friend std::ostream &operator<<(std::ostream &out, const account_name &n) {
               return out << string(n);
            }

            friend bool operator<(const account_name &a, const account_name &b) { return a.value < b.value; }

            friend bool operator<=(const account_name &a, const account_name &b) { return a.value <= b.value; }

            friend bool operator>(const account_name &a, const account_name &b) { return a.value > b.value; }

            friend bool operator>=(const account_name &a, const account_name &b) { return a.value >= b.value; }

            friend bool operator==(const account_name &a, const account_name &b) { return a.value == b.value; }

            friend bool operator==(const account_name &a, uint64_t b) { return a.value == b; }

            friend bool operator!=(const account_name &a, uint64_t b) { return a.value != b; }

            friend bool operator!=(const account_name &a, const account_name &b) { return a.value != b.value; }

            operator bool() const { return value; }

            operator uint64_t() const { return value; }

            operator unsigned __int128() const { return value; }
         };
   }
} // eosio::chain

namespace std {
   template<>
   struct hash<eosio::chain::account_name> : private hash<uint64_t> {
      typedef eosio::chain::account_name argument_type;
      typedef typename hash<uint64_t>::result_type result_type;

      result_type operator()(const argument_type &name) const noexcept {
         return hash<uint64_t>::operator()(name.get_value());
      }
   };
};

namespace fc {
   class variant;

   void to_variant(const eosio::chain::account_name &c, fc::variant &v);

   void from_variant(const fc::variant &v, eosio::chain::account_name &check);
} // fc


FC_REFLECT(eosio::chain::account_name,)
