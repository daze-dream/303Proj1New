#include "pch.h"
#include <cmath>
#include <iostream>
#include <stack>
#include <string>
#include <sstream>
using namespace std;

bool failFlag = false;
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
/*Solves operations based on what is passed.
	Because the stack created takes only char, specific letters
	had to be developed for the stack to interpret. Here is the key:
	g : greater than or equal
	l : less than or equal
	o : logical or
	a : logical and
	d : decrement
	c : increment
	n : logical not
	Not shown:
	t : negative
	All others should be self explanatory.
	Need to create default error code 
*/
int useOperator(int a, int b, char op) {
	switch (op) {
	case '+':
		return a + b;
	case '-':
		return a + -b;
	case '*':
		return a * b;
	case '/':
		if (b == 0)
		{
			cout << "Error: Division by 0" << endl;
			failFlag = true;
			break;
		}
		else
		{
			return a / b;
		}

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
	default:
		cout << "Error: Unknown operand entered." << endl;
		break;
	}
	return 0; // Need to make default error return
}
// Function that returns value of expression after evaluation. Please read thorouhgly
int evaluate(string index) {
	int i;

	// Stack for numerical values
	stack <int> values;

	// Stack for operands. Although on second thought, strings may have been better. Or even int, to take the pure values.
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
			an ASCII character to an int by offestting its value. Then, we assign the current item to val.*/
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

			/*
				The second check looks for a negative operator in the stack. If there is one on top, it's assumed
				that the number is turned negative, and then is popped.
			*/
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
				function to solve. Pay attention to precedence.
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
				It also acts as a first level deterrent against leading operands, prior to error checking. 
				Then we make two temp variables, assign them with respectively popped values from value stack, 
				and then pass to useOperator function to get a result. That result is then pushed to the value stack.
			*/
			while (!ops.empty() && precedence(ops.top()) >= precedence(index[i]) && (values.size() >=2)) {
				// default values assigned for decrement and increment operators to work with single values
				int val2 = 0;
				val2 = values.top();
				values.pop();

				// default values assigned for decrement and increment operators to work with single values
				int val1 = 0;

				// A special check is done here so that solo operators are not considered when using a second value.
				if (ops.top() != 'c' || ops.top() != 'd' || ops.top() == '!')
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
				This is extended to nearly all operators. Breakdown is provided in each appropriate section

				One optimization might be to i++ to skip pushing the comparison operator. Another optimization would be to use
				switch cases. Huge efficiency gains by checking endless if's, but lazy. Once we used one if, it cascaded down.
			*/

			//If a + is encountered..
			if (index[i] == '+')
			{
				// Checks the next item to see if it's a -, and that the next item is either a digit or parenthesis...
				if (index[i+1] == '-' && (isdigit(index[i + 2]) || index[i + 2] == '('))
				{
					// if so, pushes a regular + and a negative, since it assumes something is turning negative. Increases i
					// by 1 to skip the duplicate operand
					ops.push('+');
					ops.push('t');
					i++;
				}
				// else, if the next item is a +...
				else if(index[i + 1] == '+')
				{
					// we assume it's an increment, push and increase.
					ops.push('c');
					i++;
				}
				// if nothing else...
				else
				{
					// we assume it's a regular addition, and push that.
					ops.push('+');
				}
				continue;
			}

			// if the item is an !...
			else if (index[i] == '!')
			{
				// if the next item is =...
				if (index[i + 1] == '=')
				{
					// we assume it's a comparison, push the != letter equivalent. Increase i for the loop.
					ops.push('n');
					i++;
				}
				// if nothing...
				else
				{
					// we push a regular logical NOT.
					ops.push('!');
				}
			}

			// if the item is a & and the next one is too...
			else if (index[i] == '&' && index[i + 1] == '&')
			{
				// we push the letter equivalent for logical AND. and increase i for the loop.
				ops.push('a');
				i++;
			}

			// if the item is an | and the next item is one too...
			else if (index[i] == '|' && index[i] == '|')
			{
				// we push the letter equivalent for logical OR, and increase i for the loop.
				ops.push('o');
				i++;
			}

			// if the item is an - and the next item the same...
			else if (index[i] == '-' && index[i + 1] == '-')
			{
				// if the item after next is a digit or parenthesis, and the size of the value stack is 2 or more...
				if ((isdigit(index[i + 2]) || index[i+2] == '(') && values.size() >= 1)
				{
					// we assume that there is subtraction and a negative value, so we push - and letter equivalent for NEGATIVE, and increment
					ops.push('-');
					ops.push('t');
					// keep an eye on this for troubleshooting
					i++;
				}
				// else, if we check again, and the values stack is empty...
				else if(index[i+1] == '-' && values.empty())
				{
					// we assume that it's decrement, and push the letter equivalent for DECREMENT. This is assuming it's a single entry. Increment
					ops.push('d');
					// keep an eye on this for troubleshooting
					i++;
				}
				// else, if there's nothing that meets the criteria...
				else
				{
					// we push a regular -.
					ops.push('-');
				}
				// Keep this here for troubleshooting, because it was working fine before...
				//i++;
			}

			/*Get a load of this...
				So the issue we had to deal with was negatives. Particularly, them standing by themselves like -1, at 
				the start of an expression like x+(-5+2), or just in parenthesis by themselves like (-1), while not
				clashing against a regular minus, because so many conditions have to be taken into account. So this
				conditional looks at the following:
				If it encounters a -, AND if ((Ops is empty AND i == 0 aka start of the expression) OR 
											 (ops stack is not empty AND the top of it is an opeing 
											 parenthesis aka start of an expression))
				THEN AND ONLY THEN, will a negative operator be pushed to the stack.
				This is all thanks to: https://stackoverflow.com/questions/46861254/infix-to-postfix-for-negative-numbers
			*/
			else if (index[i] == '-' && ((ops.empty() && i == 0) || ( !ops.empty() && ops.top() == '(')))
			{

				ops.push('t');
			}
			// if we encounter a comparison operator and the next item is an =...
			else if ((index[i] == '>' || index[i] == '<' || index[i] == '=') && index[i + 1] == '=')
			{
				if (index[i] == '>' ) // if >, push g for greater than or equal
					ops.push('g');
				if (index[i] == '<') // if <, push l for less than or equal
					ops.push('l');
				if (index[i] == '=') // if =, push = for simple equality check
					ops.push('=');
				i++; // increment
			}

			/*Lastly, failing all of that criteria, push the operand normally.
				MUST CHECK if size of value is at least 1 so we dont get the 
				leading operand thing. This is a second	level redundancy just for sanity.
			*/
			else if(values.size() >= 0)
			{
				if (index[i] != '*' && index[i] != '/' && index[i] != '%' && index[i] != '^' && index[i] != '-' && index[i] != '+' && index[i] != '>' && index[i] != '<' && index[i] != '=')
				{
					cout << "Error: Unknown operand at position " << i << endl;
					failFlag = true;
					break;
				}
				else
				{
					ops.push(index[i]);
				}
			}
		}
	}

	/* At this point everything is done. Now we solve the remaining operands. Again, making sure that
		we have at least two values to use so that there isn't an error.*/
	while (!ops.empty() ) {
		int val2 = 0;
		// Special check for single unit operands again. val2 is the default value to manipulate
		if (ops.top() != 'c' && ops.top() != 'd' && ops.top() != '!' && ops.top() != 'o')
		{
			// Checks for the NEGATIVE operand
			if (ops.top() == 't')
			{
				val2 = values.top() * -1;
				values.pop();
				ops.pop();
			}
			// if not, regular assign and pop
			else
			{
				val2 = values.top();
				values.pop();
			}
		}
		// if not a single operator, push and pop
		else
		{
			val2 = values.top();
			values.pop();
		}


		int val1 = 0;
		// same check
		if (ops.top() != 'c' && ops.top() != 'd' && ops.top() != '!')
		{
			val1 = values.top();
			values.pop();
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
	int result;
	while (loop != 'N')
	{
		cout << "Enter infix expression to evaluate: " << endl;
		getline(cin, expression);
		//cout << "The answer is " << evaluate(expression) << endl;
		//  do initial check. if there's  leading closing parenthesis or operand, cout << error message and break.
		//if (expression[0] == ')' || ((!isdigit(expression[0]) && (expression[1] != '+' || (expression[1] != '-' || isdigit(expression[1]))))))
		//if(expression[0] == ')' &&   ( !isdigit(expression[0]) && (expression[1] != '+' &&  expression[1] != '-')                        ) )
		//{
		//	cout << "Error. Can't have leading closing parenthesis ')', or non urnary operand..." << endl;
		//}
		if (expression[0] == ')')
		{
			cout << "Error. Can't have leading closing parenthesis ')' (at char 0)" << endl;
		}
		else if (!isdigit(expression[0]) && expression[0] == '+' && expression[1] != '+')
		{
			cout << "Error: can't lead with binary operator (at char 0)" << endl;
		}
		else 
		{
			result = evaluate(expression);
			if (!failFlag)
			{
				cout << "The answer is " << result << endl;
			}
		}
		cout << "Continue? Input N to exit, or any other key to continue: ";
		cin >> loop;
		loop = toupper(loop);
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		failFlag = false;
		cout << endl;
	}
	system("pause");
	return 0;
}