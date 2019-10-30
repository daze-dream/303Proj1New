#include "pch.h"
#include <cmath>
#include <iostream>
#include <stack>
#include <string>
#include <sstream>
using namespace std;
//The idea behind this flag was to indicate that there had been maniuplation of i in the while loop
// that checks for more digits. However, with the usage if i+1, it is no longer necessary.
//bool leFlag = false;

// Precedence of operators. Have to be careful with comparisons.
int precedence(char op) {
	if (op == 'o') // o stands for logical or
		return 1;
	if (op == 'a') // a stands for logical and
		return 2;
	if (op == '>' || op == '<' || op == 'g' || op == 'l') // g stands for greater or equal, l is less or equal
		return 3;
	if (op == '=' ||op == 'n') // n stands for not equal
		return 4;
	if (op == '+' || op == '-')
		return 5;
	if (op == '*' || op == '/' || op == '%' )
		return 6;
	if (op == '^')
		return 7;
	if (op == 'd' || op == 'c' || op == '!') // ! here is logical not, d is decrement, c is increment
		return 8;
	return 0;
}
/*Solves operations based on what is passed. Side note, 'l' and 'g' stand for less than or equal,
	and greater than or equal respectively.
*/
int useOperator(int a, int b, char op) {
	switch (op) {
	case '+':
		return a + b;
	case '-':
		return a - b;
	case '*':
		return a * b;
	case '/':
		return a / b;
	case '^':
		return pow(a, b);
	case '=':
		return (a == b);
	case '>':
		return (a > b);
	case '<':
		return (a < b);
	case 'g':
		return (a >= b);
	case 'l':
		return (a <= b);
	case 'o':
		return (a || b);
	case 'a':
		return (a && b);
	case 'd':
		return (--b);
	case 'c':
		return (++b);
	case 'n':
		return (a != b);
	case '%':
		return (a%b);
	case '!':
		return (!b);
	}
	return 0;
}
// Function that returns value of expression after evaluation.
int evaluate(string index) {
	int i;

	// Stack for numerical values
	stack <int> values;

	// Stack for operands
	stack <char> ops;

	// Begin parsing the string. This goes on until we reach string legnth.
	for (i = 0; i < index.length(); i++) {

		// If current value is a whitespace, skip it.
		if (index[i] == ' ')
			continue;

		/*If the operand is an opening parenthesis, push to ops. Note that this doesn't support other
			types of brackets.
		*/
		else if (index[i] == '(') {
			ops.push(index[i]);
		}
		/*	If index[i] is a number (or digit according to the function), we begin to process it.
		*/
		else if (isdigit(index[i])) {
			/*We create a generic variable and give it a value, The " -'0' " portion converts it from
			an ASCII character to an int. Then, we assign the current item to val.*/
			int val = 0;
			val = index[i] - '0';

			// Used for debugging, remember to remove after testing
			//cout << val << endl;


			/*This portion makes sure that it captured the whole number, and checks if there
				are more numbers after the initial digit. It checks the next item, and if it's
				a digit, it increases val by a magnitude of 10, then tacks on the item, using 
				the same ASCII conversion. It then increases i once to get to the last digit, then
				the next loop takes care of getting it to the next item which would surely be an operator.
				One initial error encountered was that we simply did a while loop to keep processing through
				the string until we found that it was not a digit. However, i would be increased twice: once
				in the while loop, and then again in the main loop. That means that it would skip the operand
				during parsing and go into the next number. THe end result was that we had all our values in
				val, but ops was empty because they were never processed.
			*/
			while (isdigit(index[i + 1]))
			{
				val = (val * 10) + (index[i + 1] - '0');
				i++;

			}

			if (!ops.empty() && ops.top() == 't')
			{
				val = val * -1;
				ops.pop();
			}

			// After everything, push result onto val stack.
			values.push(val);
		}

		/* if we encounter a closing parenthesis, we solve everyting inside */
		else if (index[i] == ')')
		{
			/*Checks if ops stack is not empty, and that the 'top' value is not an invalid openining parenthesis.
				It then pops two values from value stack, pops an op from the op stack, and then passes it to the useOperator
				function to solve. Pay attention to precedence
			*/
			while (!ops.empty() && ops.top() != '(')
			{
				int val2 = values.top();
				values.pop();

				int val1 = values.top();
				values.pop();

				char op = ops.top();
				ops.pop();

				values.push(useOperator(val1, val2, op));
			}

			// pop opening brace at the end of it all.
			if (!ops.empty())
				ops.pop();
		}

		// If the current item is a regular operator, we begin to solve
		else
		{
			/*This loop begins by comparing precedence (refer to precendence function) of the top operator in
				the stack and the currently processing operator. It also does a check that there are two or more values in
				the value stack, so that we don't get an memory access violation (can't pop an empty stack.)
				It also acts as a first level deterrent against leading operands. Then we make two temp variables,
				assign them with respectively popped values from value stack, and then pass to useOperator function
				to get a result. That result is then pushed to the value stack.
			*/
			while (!ops.empty() && precedence(ops.top()) >= precedence(index[i]) && (values.size() >=2)) {
				int val2 = 0;
				val2 = values.top();
				values.pop();
				int val1 = 0;

				if (ops.top() != 'i' || ops.top() != 'd' || ops.top() == '!')
				{
					val1 = values.top();
					values.pop();
				}
				char op = ops.top();
				ops.pop();

				values.push(useOperator(val1, val2, op));
			}

			/*In this case, we assume that it is an operand. As such, push it to op stack.
				Here's the thing: getting >=, <=, and == to process with our method is 
				extremely difficult. This 'if' statement here, then, takes care of that.
				It first identifies if it is a comparison operator. Then, it checks if the next
				item in the string is an '='. If it is, then we have a full comparison happening.
				Here is the breakdown:
					>= pushes the char 'g' which stands for greater, letting us know that it means
						a greater or equal is being processed.
					<= pushes the char 'l', which stands for lesser, which works on the same 
						principle as above.
					== pushes the operator '=' to simplify things.
				One optimization might be to i++ to skip pushing the comparison operator.
			*/
			if (index[i] == '+')
			{
				if (index[i+1] == '-' && (isdigit(index[i + 2]) || index[i + 2] == '('))
				{

					ops.push('+');
					ops.push('t');
					i++;
				}
				else if(index[i + 1] == '+')
				{
					ops.push('c');
					i++;
				}
				else
				{
					ops.push('+');
				}
				continue;
			}

			else if (index[i] == '!')
			{
				if (index[i + 1] == '=')
				{
					ops.push('n');
					i++;
				}
				else
				{
					ops.push('!');
				}
			}

			else if (index[i] == '&' && index[i + 1] == '&')
			{
				ops.push('a');
				i++;
			}

			else if (index[i] == '|' && index[i] == '|')
			{
				ops.push('o');
				i++;
			}
			else if (index[i] == '-' && index[i + 1] == '-')
			{
				if ((isdigit(index[i + 2]) || index[i+2] == '(') && values.size() >= 2)
				{

					ops.push('-');
					ops.push('t');
				}
				else
				{
					ops.push('d');
				}

				i++;
			}
			else if ((index[i] == '>' || index[i] == '<' || index[i] == '=') && index[i + 1] == '=')
			{
				if (index[i] == '>' )
					ops.push('g');
				if (index[i] == '<')
					ops.push('l');
				if (index[i] == '=')
					ops.push('=');
				i++;
			}

			/*MUST CHECK if size of value is at least 1 so we dont get the leading operand thing. This is a second
				level redundancy just for sanity.
			*/
			else if(values.size() >= 1)
			{
				ops.push(index[i]);
			}
		}
	}

	/* At this point everything is done. Now we solve the remaining operands. Again, making sure that
		we have at least two values to use so that there isn't an error.*/
	while (!ops.empty() ) {
		int val2 = 0;
		val2 = values.top();
		values.pop();

		int val1 = 0;
		if (ops.top() != 'c' && ops.top() != 'd' && ops.top() != '!' && ops.top() != 'o' )
		{
			if(ops.top() == '-')
			val1 = values.top() * - 1;
			values.pop();
			ops.pop();
		}

		char op = ops.top();
		ops.pop();

		values.push(useOperator(val1, val2, op));
	}

	// The stack now solely contains the result of all this mess. So we return it.
	return values.top();
}

int main() {
	string expression;
	char loop = ' ';
	while (loop != 'N')
	{
		cout << "Enter infix expression to evaluate: " << endl;
		getline(cin, expression);
		cout << "The answer is " << evaluate(expression) << endl;
		cout << "Continue? Input N to exit, or any other key to continue: ";
		cin >> loop;
		loop = toupper(loop);
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << endl;
	}
	system("pause");
	return 0;
}