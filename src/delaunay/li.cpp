#include "li.h"

#include <cstdlib>

#include <iostream>
#include <sstream>



/*********************************************************CONSTRUCTORS*********************************************************/
LongInt::LongInt() {

}



LongInt::LongInt(const LongInt& oldLongInt) {
	//Copy vector
	using std::vector;
	this->myLongInt = oldLongInt.myLongInt;

	//Copy sign
	this->signType = oldLongInt.signType;
}



LongInt::LongInt(int x) {
	//Check sign of int
	if (x == 0)
		signType = type_zero;
	else if (x > 0)
		signType = type_positive;
	else if (x < 0)
		signType = type_negative;

	//Convert int into a vector of longs of nine digits each
	long y = abs(x);

	do {
		long remainder = (y % DIVISOR);
		myLongInt.push_back(remainder);
		y = y / DIVISOR;
	} while (y > 0);
}



LongInt::LongInt(std::string x){
	//Check sign of number
	if (x[0] == '0')
		this->signType = type_zero;
	else if (x[0] == '-') {
		this->signType = type_negative;
		x = x.substr(1, x.length()-1);
	}
	else
		this->signType = type_positive;

	int len = x.length();
	this->myLongInt.clear();

	if (len > 9) {
		this->myLongInt.push_back(atol(x.substr(len-9, 9).c_str()));

		for (int i = 1; i < len / 9; i ++) {
			this->myLongInt.push_back(atol(x.substr(len-(9*(i+1)), 9).c_str()));
		}

		if (len % 9 != 0) {
			this->myLongInt.push_back(atol(x.substr(0, len % 9).c_str()));
		}
	} else {
		this->myLongInt.push_back(atol(x.c_str()));
	}
}



/*********************************************************PRIVATE METHODS*********************************************************/
LongInt LongInt::plusHelper(const LongInt& newNumber) const {
	long carryover = 0L;
	LongInt theAnswer;

	theAnswer.signType = signType;

	unsigned int i;

	if (myLongInt.size() == newNumber.myLongInt.size()) {
		for (i = 0; i < newNumber.myLongInt.size(); i++) {
			long resultant = myLongInt[i] + newNumber.myLongInt[i] + carryover;
			carryover = resultant/DIVISOR;
			resultant = (resultant % DIVISOR);
			theAnswer.myLongInt.push_back(resultant);
		}

		if (carryover > 0L) {
			theAnswer.myLongInt.push_back(carryover);
		}
	} else if (myLongInt.size() > newNumber.myLongInt.size()) {
		for (i = 0; i < newNumber.myLongInt.size(); i++) {
			long resultant = myLongInt[i] + newNumber.myLongInt[i] + carryover;
			carryover = resultant / DIVISOR;
			resultant = (resultant % DIVISOR);
			theAnswer.myLongInt.push_back(resultant);
		}

		for (unsigned int j = i; j < myLongInt.size(); j++) {
			long resultant = myLongInt[j] + carryover;
			carryover = resultant / DIVISOR;
			resultant = (resultant % DIVISOR);
			theAnswer.myLongInt.push_back(resultant);
		}

		if (carryover > 0L) {
			theAnswer.myLongInt.push_back(carryover);
		}
	} else {
		for (i = 0; i < myLongInt.size(); i++) {
			long resultant = myLongInt[i] + newNumber.myLongInt[i] + carryover;
			carryover = resultant / DIVISOR;
			resultant = (resultant % DIVISOR);
			theAnswer.myLongInt.push_back(resultant);
		}

		for (unsigned int j = i; j < newNumber.myLongInt.size(); j++) {
			long resultant = newNumber.myLongInt[j] + carryover;
			carryover = resultant / DIVISOR;
			resultant = (resultant % DIVISOR);
			theAnswer.myLongInt.push_back(resultant);
		}

		if (carryover > 0L) {
			theAnswer.myLongInt.push_back(carryover);
		}
	}

	return theAnswer;
}



LongInt LongInt::minusHelper(const LongInt& newNumber) const {
	long carryover = 0L;

	LongInt theAnswer;
	theAnswer.signType = type_positive;

	unsigned int i;
	long resultant;

	if (myLongInt.size() == newNumber.myLongInt.size()) {
		if (*this > newNumber) {
			for (i = 0; i < newNumber.myLongInt.size(); i++) {
				if (myLongInt[i] >= (newNumber.myLongInt[i] + carryover)) {
					resultant = myLongInt[i] - newNumber.myLongInt[i] - carryover;
					carryover = 0L;
				} else {
					resultant = DIVISOR + myLongInt[i] - newNumber.myLongInt[i] - carryover;
					carryover = 1L;
				}

				theAnswer.myLongInt.push_back(resultant);
			}
		} else {
			for (i = 0; i < newNumber.myLongInt.size(); i++) {
				if (newNumber.myLongInt[i] >= (myLongInt[i] + carryover)) {
					resultant = newNumber.myLongInt[i] - myLongInt[i] - carryover;
					carryover = 0L;
				} else {
					resultant = DIVISOR + newNumber.myLongInt[i] - myLongInt[i] - carryover;
					carryover = 1L;
				}

				theAnswer.myLongInt.push_back(resultant);
			}
			theAnswer.signType = type_negative;
		}
	} else if (myLongInt.size() > newNumber.myLongInt.size()) {
		for (i = 0; i < newNumber.myLongInt.size(); i++) {
			if (myLongInt[i] >= (newNumber.myLongInt[i] + carryover)) {
				resultant = myLongInt[i] - newNumber.myLongInt[i] - carryover;
				carryover = 0L;
			} else {
				resultant = DIVISOR + myLongInt[i] - newNumber.myLongInt[i] - carryover;
				carryover = 1L;
			}

			theAnswer.myLongInt.push_back(resultant);
		}

		for (unsigned int j = i; j < myLongInt.size(); j++) {
			if (carryover == 1L && myLongInt[j] == 0L) {
				resultant = DIVISOR - carryover;
				carryover = 1L;
			} else {
				resultant = myLongInt[j] - carryover;
				carryover = 0L;
			}

			theAnswer.myLongInt.push_back(resultant);
		}
	} else {
		for (i = 0; i < myLongInt.size(); i++) {
			if (newNumber.myLongInt[i] >= (myLongInt[i] + carryover)) {
				resultant = newNumber.myLongInt[i] - myLongInt[i] - carryover;
				carryover = 0L;
			} else {
				resultant = DIVISOR + newNumber.myLongInt[i] - myLongInt[i] - carryover;
				carryover = 1L;
			}

			theAnswer.myLongInt.push_back(resultant);
		}

		for (unsigned int j = i; j < newNumber.myLongInt.size(); j++) {
			if (carryover == 1L && newNumber.myLongInt[j] == 0L) {
				resultant = DIVISOR - carryover;
				carryover = 1L;
			} else {
				resultant = newNumber.myLongInt[j] - carryover;
				carryover = 0L;
			}

			theAnswer.myLongInt.push_back(resultant);
		}

		theAnswer.signType = type_negative;
	}

	// removing zeros
	bool zero = true;
	for (int k = (theAnswer.myLongInt.size() - 1); k >= 0 && zero; k--) {
		if (theAnswer.myLongInt[k] != 0L) {
			zero = false;
		} else {
			theAnswer.myLongInt.erase(theAnswer.myLongInt.end() - 1);
		}
	}

	if (zero) {
		theAnswer.setZero_();
	}

	return theAnswer;
}



void LongInt::assignWithCheck(LongInt otherLongInt, int start, int end) {
	bool isZero = true;
	if (start != end) {
		long toBePushed;

		for (int i = start; i < end; i++) {
			toBePushed = otherLongInt.myLongInt[i];

			if (toBePushed != 0)
				isZero = false;

			this->myLongInt.push_back(toBePushed);
		}
	}
	if (isZero)
		this->setZero_();
	else
		this->signType = type_positive;
}



LongInt LongInt::longMult(const LongInt& otherLongInt) const {
	LongInt theAnswer = LongInt(0);

	if (!this->eqZero() && !otherLongInt.eqZero()) {
		int thisSize = (int) this->myLongInt.size();
		int otherSize = (int) otherLongInt.myLongInt.size();
		LongInt multTemp;
		bool isMultTempValued = false;

		for (int j = 0; j < otherSize; j++) { // j is the iterator through otherLongInt
			long long mult2 = otherLongInt.myLongInt[j];
			multTemp.setZero_();

			for (int k = 0; k <= j-1; k++) {
				multTemp.myLongInt.push_back(0);
			}

			for (int i = 0; i < thisSize; i++) { // i is the iterator through this
				long long mult1 = this->myLongInt[i];
				long long multResult = mult1 * mult2;

				// There might be overflow from the previous multiplication
				multResult += multTemp.myLongInt[j+i];

				if (!isMultTempValued && (multResult > 0))
					isMultTempValued = true;

				// We check for overflow
				long exceed = multResult / DIVISOR;

				if (exceed > 0)
					multResult %= DIVISOR; // If overflow take remainder

				multTemp.myLongInt[j+i] = (long) multResult;

				if ((i != thisSize-1) || (exceed > 0))
					multTemp.myLongInt.push_back(exceed);
			}

			if (isMultTempValued)
				multTemp.signType = type_positive;

			theAnswer = theAnswer + multTemp;
		}
	}

	return theAnswer;
}



// Karatsuba Multiplication
// The number of 'digits' we're looking at here is the number of elements in the array.
// Base 10.
LongInt LongInt::karatsuba(const LongInt& otherLongInt) const {
	LongInt theAnswer;

	int thisSize = (int) this->myLongInt.size();
	int otherSize = (int) otherLongInt.myLongInt.size();

	if (thisSize < KARAT_MIN_LENGTH || otherSize < KARAT_MIN_LENGTH) {
		theAnswer = this->longMult(otherLongInt);
	} else { // Begin algorithm
		int digits = std::max(thisSize, otherSize)/2;
		// Calculate length of shifting numbers' sizes
		int x1_size = thisSize - digits;

		if (x1_size < 0)
			x1_size = 0;

		int y1_size = otherSize - digits;

		if (y1_size < 0)
			y1_size = 0;

		using std::vector;

		int xStart = 0;
		int xEnd = thisSize;
		int yStart = 0;
		int yEnd = otherSize;

		int xMiddle = xStart + (thisSize - x1_size);
		int yMiddle = yStart + (thisSize - y1_size);

		LongInt x1, x2, y1, y2;

		x2 = LongInt();
		x2.assignWithCheck((*this), xStart, xMiddle);
		y2 = LongInt();
		y2.assignWithCheck(otherLongInt, yStart, yMiddle);

		x1 = LongInt();
		x1.assignWithCheck((*this), xMiddle, xEnd);
		y1 = LongInt();
		y1.assignWithCheck(otherLongInt, yMiddle, yEnd);

		LongInt z2 = x1.karatsuba(y1);
		LongInt z0 = x2.karatsuba(y2);
		LongInt z1 = (x1+x2).karatsuba(y1+y2) - z2 - z0;

		z2.myLongInt.insert(z2.myLongInt.begin(), 2*digits, 0);
		z1.myLongInt.insert(z1.myLongInt.begin(), digits, 0);

		LongInt temp = z1 + z0;
		theAnswer = z2 + temp;
	}

	return theAnswer;
}



void LongInt::changeSign() {
	switch (signType) {
		case type_positive:
			signType = type_negative;
			break;
		case type_negative:
			signType = type_positive;
			break;
		default:
			break;
	}
}



/*********************************************************PROTECTED METHODS*********************************************************/
void LongInt::setZero_() {
	signType = type_zero;
	myLongInt.clear();
	myLongInt.push_back(0);
}



/*********************************************************PUBLIC METHODS*********************************************************/
void LongInt::dump() const {
	using std::cout;
	using std::endl;

	if (this->eqZero())
		cout << 0 << endl;
	else {
		for (int i = (myLongInt.size() - 1); i >= 0; i--) {
			cout << myLongInt[i];
		}
		cout << endl;
	}
}



std::string LongInt::printOut() {
	std::string answer;
	std::stringstream ss;
	bool toAppend = false;

	if (myLongInt[(myLongInt.size() - 1)] != 0L) {
		ss << myLongInt[(myLongInt.size() - 1)];
		toAppend = true;
	}

	for (int i = (myLongInt.size() - 2); i >= 0; i--) {
		if (myLongInt[i] != 0L || toAppend) {
			toAppend = true;
			std::stringstream tss;
			tss << myLongInt[i];
			std::string valueToAppend;
			tss >> valueToAppend;
			int amountToPad = 9 - valueToAppend.length();
			std::string padding = "";

			while (amountToPad != 0){
				padding = padding + "0";
				amountToPad--;
			}

			ss << padding + valueToAppend;
		}
	}

	ss >> answer;

	if (answer == "") {
		answer = "0";
	}

	if (signType == type_negative) {
		answer = "-" + answer;
	}

	return answer;
}



/*
LongInt& LongInt::operator=(int x) {
	LongInt theX = LongInt(x);
	myLongInt = theX.myLongInt;
	signType = theX.signType;
	return LongInt(x);
}
*/



LongInt& LongInt::operator=(const LongInt& x) {
	myLongInt = x.myLongInt;
	signType = x.signType;
	return *this;
}



LongInt LongInt::operator+(const LongInt& newNumber) const { // currentValue + newNumber
	if (this->sign() == 0) {
		return newNumber;
	} else if (newNumber.sign() == 0) {
		return *this;
	}

	if (this->sign() < 0 && newNumber.sign() > 0) { // (-currentLongInt) + (newNumber)
		LongInt theAnswer = *this;
		theAnswer.changeSign();                                         // (currentLongInt), (newNumber)
		return newNumber.minusHelper(theAnswer);                        // (newNumber) - (currentLongInt)
	} else if (this->sign() > 0 && newNumber.sign() < 0) {      // (currentLongInt) + (-newNumber)
		LongInt theAnswer = newNumber;
		theAnswer.changeSign();                                     // (currentLongInt), (newNumber)
		return this->minusHelper(theAnswer);                        // (currentLongInt) - (newNumber)
	}

	return plusHelper(newNumber);
}



LongInt LongInt::operator-(const LongInt& newNumber) const { // currentValue - newNumber
	if (this->sign() == 0) {
		LongInt theAnswer = newNumber;
		theAnswer.changeSign();
		return theAnswer;
	} else if (newNumber.sign() == 0) {
		return *this;
	}

	if (this->sign() > 0 && newNumber.sign() < 0) { // (currentLongInt) - (-newNumber)
		LongInt theAnswer = newNumber;
		theAnswer.changeSign();              // (currentLongInt), (newNumber)
		return this->plusHelper(theAnswer);  // (currentLongInt) + (newNumber)
	} else if (this->sign() < 0 && newNumber.sign() > 0) { // (-currentLongInt) - (newNumber)
		LongInt theAnswer = newNumber;
		theAnswer.changeSign();             // (-currentLongInt), (-newNumber)
		return this->plusHelper(theAnswer); // (-currentLongInt) + (-newNumber)
	} else if (this->sign() < 0 && newNumber.sign() < 0) { // (-currentLongInt) - (-newNumber)
		LongInt theAnswer = newNumber;
		theAnswer.changeSign(); // (-currentLongInt), (newNumber)
		LongInt originalNumber = *this;
		originalNumber.changeSign(); // (currentLongInt), (newNumber)
		return theAnswer.minusHelper(originalNumber); // (newNumber) - (currentLongInt)
	}

	return minusHelper(newNumber);
}



LongInt LongInt::operator*(const LongInt& otherLongInt) const {
	LongInt theAnswer;

	if (this->eqZero() || otherLongInt.eqZero()) {
		theAnswer.setZero_();
	} else {
		int thisSign = this->sign();
		int otherSign = otherLongInt.sign();

		theAnswer = this->karatsuba(otherLongInt);

		if (thisSign == otherSign)
			theAnswer.signType = type_positive;
		else
			theAnswer.signType = type_negative;
	}

	return theAnswer;
}



bool LongInt::operator>(const LongInt& otherLongInt) const {
	int thisSign = this->sign();
	int otherSign = otherLongInt.sign();

	int thisSize = (int)this->myLongInt.size();
	int otherSize = (int)otherLongInt.myLongInt.size();

	if (thisSign > otherSign)
		return true; //Early return for optimization
	else if (thisSign < otherSign)
		return false; //Early return for optimization
	else if (this->eqZero() && otherLongInt.eqZero())
		return false; //Early return for optimization
	else if (((thisSign == 1) && (thisSize < otherSize)) || ((thisSign == -1) && (thisSize > otherSize)))
		return false; //If both are positive or both are negative
	else if (((thisSign == 1) && (thisSize > otherSize)) || ((thisSign == -1) && (thisSize < otherSize)))
		return true; //If both are positive or both are negative
	else { //Same sign and same size now
		for (int i = (thisSize - 1); i >= 0; i--){
			//Starting from most significant figure
			long thisLong = this->myLongInt[i];
			long otherLong = otherLongInt.myLongInt[i];

			if (((thisSign == 1) && (thisLong < otherLong))      //Both positive
			    || ((thisSign == -1) && (thisLong > otherLong))) //Both negative
				return false;
			if (((thisSign == 1) && (thisLong > otherLong))       //Both positive
			     || ((thisSign == -1) && (thisLong < otherLong))) //Both negative
				return true;
		}
	}

	return false;
}



bool LongInt::operator<(const LongInt& otherLongInt) const {
	return otherLongInt.operator>(*this);
}



bool LongInt::operator==(const LongInt& otherLongInt) const {
	int thisSign = this->sign();
	int otherSign = otherLongInt.sign();

	int thisSize = (int)this->myLongInt.size();
	int otherSize = (int)otherLongInt.myLongInt.size();

	if ((thisSign != otherSign) || (thisSize != otherSize))
		return false; //Early return for optimization
	else {
		for (int i = 0; i < thisSize; i++){
			if (this->myLongInt[i] != otherLongInt.myLongInt[i])
				return false; //Early return for optimization
		}
	}

	return true;
}



bool LongInt::eqZero() const {
	return (signType == type_zero);
}



int LongInt::sign() const {
	switch (signType) {
		case type_positive:
			return 1;
		case type_negative:
			return (-1);
		case type_zero:
			return 0;
		default: return 0;
	}
}



double LongInt::doubleValue() const {
	if (this->eqZero())
		return 0;

	double doubleValue = 0;

	for (int i = this->myLongInt.size() - 1; i >= 0; i--) {
		doubleValue = doubleValue * DIVISOR + this->myLongInt[i];
	}

	if (signType == type_negative)
		doubleValue = -doubleValue;

	return doubleValue;
}



std::ostream& operator<<(std::ostream& os, const LongInt& li) {
	os << li.doubleValue();
	return os;
}



/* ---------------------------
    Unary Operator
--------------------------- */
LongInt operator-(LongInt& theLongInt) {
	LongInt theAnswer = LongInt(theLongInt);
	theAnswer.changeSign();
	return theAnswer;
}

