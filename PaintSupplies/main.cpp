#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string>
#include <thread>

using namespace std;

bool terminateProgram = false;

const string CODES[5] = {
    "A1", "A2", "A3", "A4", "A5",
};

const string DESCS[5] = {
    "Boysen (1L) White", "Boysen (2L) Orange", "Boysen (4L) Black", "Boysen (8L) Yellow", "Boysen (16L) Green",
};

const float PRICES[5] = {
    500.21, 200.00, 600.00, 517.26, 250.75,
};

void gotoxy(short x, short y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

struct Orders {
    int** items;
    int size;
    double total;
};

int getYPos() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(hConsole, &csbi);

    return csbi.dwCursorPosition.Y - 1;
}

void clearArea(short y, short lines = 1, short x = 0) {
    DWORD written;
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD size = { 120, lines };
    COORD pos = { x, y };
    FillConsoleOutputCharacter(handle, ' ', size.X * size.Y, pos, &written);
}

void checkF1() {
    while (!terminateProgram) {
        if (GetAsyncKeyState(VK_F1) & 0x8000) {
            clearArea(5, 20);
            gotoxy(5, 5);
            exit(0);
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void listenF1() {
    thread f1Thread(checkF1);
    f1Thread.detach();  // Detach the thread to run independently
}

void welcome() {
    gotoxy(50, 1);
    cout << "Paint Supplies";
    gotoxy(5, 2);
    cout << CODES[0] + ": " + DESCS[0] + " @ Php " << PRICES[0];
    gotoxy(5, 3);
    cout << CODES[1] + ": " + DESCS[1] + " @ Php " << PRICES[1];
    gotoxy(45, 2);
    cout << CODES[2] + ": " + DESCS[2] + " @ Php " << PRICES[2];
    gotoxy(45, 3);
    cout << CODES[3] + ": " + DESCS[3] + " @ Php " << PRICES[3];
    gotoxy(85, 2);
    cout << CODES[4] + ": " + DESCS[4] + " @ Php " << PRICES[4];
    gotoxy(85, 3);
    cout << "F1 - Exit";
}

void displayHeader() {
    gotoxy(5, 5);
    cout << "ITEM CODE";
    gotoxy(5, 6);
    cout << "---------------";

    gotoxy(30, 5);
    cout << "DESCRIPTION";
    gotoxy(30, 6);
    cout << "--------------";

    gotoxy(55, 5);
    cout << "UNIT PRICE";
    gotoxy(55, 6);
    cout << "--------------";

    gotoxy(80, 5);
    cout << "QTY";
    gotoxy(80, 6);
    cout << "--------------";

    gotoxy(105, 5);
    cout << "AMOUNT";
    gotoxy(105, 6);
    cout << "-------------";
}

short getQty(int y) {
    short qty;
    while (true) {
        gotoxy(80, y);
        if (cin >> qty) {
            break;
        }
        else {
            cin.clear();
            cin.ignore(32767, '\n');
            clearArea(y, 1, 80);
        }
    }
    return qty;
}

short getIndex(string code) {
    for (short i = 0; i < 5; i++) {
        if (CODES[i] == code) {
            return i;
        }
    }
    return -1;
}

short getCode() {
    string code;
    cin.clear();
    getline(cin, code);

    if (code.empty()) return -2;

    int i = getIndex(code);

    return i;
}

void displayDetails(const short index, int y) {
    gotoxy(30, y);
    cout << DESCS[index];
    gotoxy(55, y);
    cout << "Php " << PRICES[index];
}

void displayAmount(const double amount, int y) {
    gotoxy(105, y);
    cout << "Php " << amount;
}

Orders addToOrders(Orders orders, int* item) {
    int newSize = orders.size + 1;
    int** newOrders = new int* [newSize];

    if (orders.items != nullptr) {
        for (int i = 0; i < orders.size; ++i) {
            newOrders[i] = new int[2];
            newOrders[i][0] = orders.items[i][0];
            newOrders[i][1] = orders.items[i][1];
            delete[] orders.items[i];
        }
        delete[] orders.items;
    }

    newOrders[orders.size] = new int[2];
    newOrders[orders.size][0] = item[0];
    newOrders[orders.size][1] = item[1];

    double price = PRICES[item[0]];
    double subTotal = price * item[1];
    double newTotal = orders.total + subTotal;

    Orders result = { newOrders, newSize, newTotal };
    return result;
}

void displayTotal(int yPos, double total = 0.0) {
    clearArea(yPos - 1, 3);
    gotoxy(105, yPos);
    cout << "Php " << total;
    gotoxy(105, yPos + 1);
    cout << "-------------";
    gotoxy(105, yPos + 2);
    cout << "TOTAL ";
    gotoxy(5, yPos + 2);
    cout << "Press Enter to end the transaction... ";
}

void endTransaction(const Orders& orders) {
    clearArea(5, 20);
    gotoxy(5, 5);
    cout << "Transaction Summary";
    gotoxy(10, 7);
    cout << "QTY";
    gotoxy(15, 7);
    cout << "| Code";
    gotoxy(22, 7);
    cout << "| Description";
    gotoxy(45, 7);
    cout << "| Amount";
    gotoxy(10, 8);
    cout << "-----|------|----------------------|-----------";
    for (int i = 0; i < orders.size; ++i) {

        gotoxy(10, i + 9);
        cout << orders.items[i][1];
        gotoxy(15, i + 9);
        cout << "| " << CODES[orders.items[i][0]];
        gotoxy(22, i + 9);
        cout << "| " << DESCS[orders.items[i][0]];

        gotoxy(45, i + 9);
        cout << "| Php " << PRICES[orders.items[i][0]] * orders.items[i][1];
    }
    gotoxy(10, getYPos() + 2);
    cout << "-----|------|----------------------|-----------";
    gotoxy(39, getYPos() + 2);
    cout << "TOTAL" << " | Php " << orders.total;

    gotoxy(5, getYPos() + 3);
    cout << "Please pay this amount: Php " << orders.total;
}

void displayDate() {
    time_t currentTime;
    time(&currentTime);
    tm localTime = {};

    localtime_s(&localTime, &currentTime);

    char buffer[80];

    strftime(buffer, sizeof(buffer), "%m/%d/%Y", &localTime);

    gotoxy(5, 0);
    cout << "Date: " << buffer << endl;
}

int main() {
    Orders orders = { nullptr, 0, 0 };
    listenF1();
    displayDate();
    welcome();

    while (true) {
        clearArea(5, 50);

        while (!terminateProgram) {
            displayHeader();
            gotoxy(5, 7 + orders.size);
            int i = getCode();

            if (i != -2) {
                if (i != -1) {
                    displayDetails(i, getYPos());
                    int qty = getQty(getYPos() + 1);
                    double amt = PRICES[i] * qty;
                    displayAmount(amt, getYPos());

                    int newItem[2] = { i, qty };
                    orders = addToOrders(orders, newItem);
                    displayTotal(8 + orders.size, orders.total);
                    cin.ignore(32767, '\n');
                }
                else {
                    clearArea(getYPos());
                }
            }
            else if (orders.size != 0) {
                break;
            }
        }

        endTransaction(orders);
        orders = { nullptr, 0, 0 };

        gotoxy(5, getYPos() + 4);
        cout << "[F2] Another Entry/Customer | [F10] Terminate...";

        int key = _getch();

        while (true) {
            if (GetAsyncKeyState(VK_F2) & 0x8000) {
                break;
            }
            else if (GetAsyncKeyState(VK_F10) & 0x8000) {    
				if (orders.items != nullptr) {
				    for (int i = 0; i < orders.size; ++i) {
				        delete[] orders.items[i];
				    }
				    delete[] orders.items;
				}
				
                exit(0);
            }
        }
    }

    return 0;
}
