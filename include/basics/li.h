#ifndef LIH
#define LIH

#include <cstdio>
#include <string>
#include <vector>

//Const definitions
const long DIVISOR = 1000000000;
const int KARAT_MIN_LENGTH = 121;

class LongInt {
	private:
		enum SignType {type_positive, type_negative, type_zero};
		SignType signType;
		std::vector<long> myLongInt;

		//Helper functions
		LongInt plusHelper(const LongInt& currentLongInt) const;
		LongInt minusHelper(const LongInt& currentLongInt) const;
		void assignWithCheck(LongInt, int, int);
		LongInt longMult(const LongInt&) const;
		LongInt karatsuba(const LongInt&) const;
		void changeSign();

	protected:
		void setZero_(); // set this number to be zero

	public:
		LongInt();
		LongInt(const LongInt&); // copy constructor
		LongInt(int); // take in an integer
		LongInt(std::string x); // take in a string
		void dump() const; // print out this number
		std::string printOut(); // convert number to a string

		LongInt& operator=(int i) {return (*this) = LongInt(i);};
		LongInt& operator=(const LongInt&);
		LongInt operator+(const LongInt&) const;
		LongInt operator+(int i) const {return (*this) + LongInt(i);};
		LongInt operator-(const LongInt&) const;
		LongInt operator-(int i) const {return (*this) + LongInt(i);};
		LongInt operator*(const LongInt&) const;
		LongInt operator*(int i) const {return (*this) * LongInt(i);};

		bool operator>(const LongInt&) const;
		bool operator>(int i) const {return ((*this) > LongInt(i));};
		bool operator<(const LongInt&) const;
		bool operator<(int i) const {return ((*this) < LongInt(i));};
		bool operator==(const LongInt&) const;

		bool operator>=(const LongInt& oLT) const {return ((*this) == oLT) || ((*this) > oLT);};
		bool operator>=(int i) const {return ((*this) >= LongInt(i));};
		bool operator<=(const LongInt& oLT) const {return ((*this) == oLT) || ((*this) < oLT);};
		bool operator<=(int i) const {return ((*this) <= LongInt(i));};

		bool eqZero() const;  // return true if it is zero
		int sign() const;     // return +1 if it's positive, -1 if it's negative, 0 if it's zero

		double doubleValue() const; // return a double approximation (you can assume that it will not be out of bound)

	friend LongInt operator-(LongInt&);
};

LongInt operator-(LongInt&); // negative sign


#endif
