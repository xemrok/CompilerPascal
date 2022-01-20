#include <QCoreApplication>
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

const int size_massiv_prog = 2048; // максимальный размер загружаемой программы
char massiv_prog[size_massiv_prog]; // массив в который будет загружена програма
string mkeywords[]={"program","var","integer","begin","if","then","else","end","end.",
                    "Program","Var","Integer","Begin","If","Then","Else","End","End."}; // массив ключевых слов
string mlogical_operators[]={"<",">","<=",">=","=","or","and","<>","xor"}; // массив логических операторов
string marithmetic_operators[]={"+",":=","*","/","-"}; // массив матиматических операторов
string mspecial_symbols[]={",",";","(",")","'",":"}; // массив специальных символов
string mnumbers[sizeof(massiv_prog)]; // массив распознанных чисел во входном тексте
string syntax[sizeof(massiv_prog)][2]; // массив синтаксиса
string obivlenie[sizeof(massiv_prog)]; // массив обявленных переменных
char mlogical_operators_1[]={'<','>','=',':'}; // массив логических симолов для подстановки пробелов
char marithmetic_operators_1[]={'-','+','*','/'}; // массив математических симолов для подстановки пробелов
char mspecial_symbols_1[] = {',',';','{','}','(',')',':'}; // массив специальных симолов для подстановки пробелов
string temporary; // временная переменная в которую загружается по символьно слово от пробела до пробела для дальнейшего распознания
int primary_element_word = 0; // начало распознанного слова
int final_element_word=0; // конец распознанного слова
bool word = false; // переменная сигнализирующая что найденное слово является одним из ключевых
int iteration_syntax = 0; // переменная для загрузки лексем в массив синтаксиса
int nums = 0; // переменная для загрузки обявленных переменных
int digit=0; // колличество найденных чисел во водном тексте
int wrd = 0; //флаг: если 0 то ненайден если 1 то число если 2 то буква
int begincheck = 0; // переменная для обнаружения открытых операторных скобок
int sl=0; // переменная для прохождения по массиву синтаксиса
int cpp = 0; // переменная для создания временных переменных в генераторе кода
int write_sl[2]; // массив хранения начального номера элемента уравнения и конечного
int skobka_counter = 0; // подсчёт скобок
int mathtriegger = 0;
int metka = 0; // переменная для создания имяни меток перехода в ассемблере
int semlogic[256];

void lex(); // обявления функции лексики
void synx(); // обявление функции синтаксиса
void published(string num); // обявление функции проверки обявленности переменной
void error_message(int error, string num); // обявление функции вывода ошибок
void ass(string asss, int asik); // обявление функции вывода асемблерного кода

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_ALL, "Russian");
    ifstream fin("program.txt", std::ios::in); // открытие файла с програмой
    char character; // переменная для загрузки символа из файла с программой
    int number_simbols_file = -1; // счётчик элементов файла
    int b = -1;
    bool clarification = false;
    bool flag = false;
    bool grab = false;
    while (!fin.eof()) // Загрузка содержимиого файла в массив с добавлением пробелов для нахождения ключивых слов
    {
        number_simbols_file++;
        if (number_simbols_file > size_massiv_prog-1) // цикл предохранения возникновения кретической ошибки
        {
            error_message(23,syntax[sl][0]); // прерывание выполнения программы если колличество символов превысит допустимый максимум
        }
        character = fin.get();
        for (int q=0; q<4;q++)
        {
            if (character == mlogical_operators_1[q])
            {
                if(clarification == true)
                {
                    massiv_prog[number_simbols_file] = character;
                    number_simbols_file++;
                    massiv_prog[number_simbols_file] = ' ';
                    clarification = false;
                    flag = true;
                }
                else
                {
                    massiv_prog[number_simbols_file] = ' ';
                    number_simbols_file++;
                    massiv_prog[number_simbols_file] = character;
                    clarification = true;
                    flag =true;
                }
            }
        }

        if (flag == false)
        {
            for (int i=0; i<8;i++)
            {
                if ((character == mspecial_symbols_1[i] || character == marithmetic_operators_1[i]) || (clarification == true && character != mspecial_symbols_1[i] && character != marithmetic_operators_1[i] && grab == false))
                {
                    massiv_prog[number_simbols_file]=' ';
                    number_simbols_file++;
                    b = i;
                    grab = true;
                }
            }
            massiv_prog[number_simbols_file]= character;
            if ((character == mspecial_symbols_1[b] && b != (-1)) || (character == marithmetic_operators_1[b] && b != (-1)))
            {
                number_simbols_file++;
                massiv_prog[number_simbols_file]=' ';
                b=-1;
            }
        }
        if(flag == false && clarification == true) {clarification = false;}
        flag=false;
        grab = false;
    }
    fin.close(); // Окончание загрузки файла в массив
    massiv_prog[number_simbols_file] = ' '; // принудительное добавление пробела в конец написанной программы
    number_simbols_file++;
    int j=0;
    for (j = 0; j<number_simbols_file ;j++) // Определение - являеться ли символ числом и запись его в отдельный массив
    {
        cout << massiv_prog[j];
        if (isdigit(massiv_prog[j]))
        {
            if (isdigit(massiv_prog[j-1]) )
            {
                mnumbers[digit-1] = mnumbers[digit-1]+massiv_prog[j];
            }
            else
            {
                mnumbers[digit] = massiv_prog[j];
                digit++;
            }
        }
    } // окончание записи чисел
     cout << endl << endl;
    for (;final_element_word<number_simbols_file; final_element_word++) // цикл распознования ключевых слов и переменных
    {
        if (massiv_prog[final_element_word] == ' ' || massiv_prog[final_element_word] == '\n')
        {
            for (;primary_element_word<final_element_word; primary_element_word++)
            {
                temporary += massiv_prog[primary_element_word];
            }
            lex(); // вызов функции для проверки какое слово мы в данный момент видим
            if (temporary != " " && word == false) // если слово небыло опознано, то считаем что это переменная
            {
                syntax[iteration_syntax][0] = "APP";
                syntax[iteration_syntax][1] = temporary;
                iteration_syntax++;
            }
            temporary = ""; // обнуляем временную переменную
            while(massiv_prog[final_element_word] == ' ' || massiv_prog[final_element_word] == '\n') // пропуск неучитываемых пробелов
            {
                final_element_word++;
            }
            primary_element_word = final_element_word;
            word = false;
        }
        if (massiv_prog[final_element_word] == '{') // если найден данный символ пропускать элементы до того как не будет найден символ }
        {
            while (massiv_prog[final_element_word] != '}')
            {
                final_element_word++;
            }
            final_element_word++;
            primary_element_word = final_element_word;
        }
    } // окончание цикла распознования элементов
    cout << endl << endl;
    synx(); // вызов функции синтаксиса
    system("pause"); // выход из программы по нажатию Enter
    return 0;
}

void lex()     // Определение к какой категории относиться текст
{
    if (word == false) // если слово ещё небыло найдено выполняем поиск по массиву ключевых слов
    {
        for (int l = 0; l<19 ; l++)
        {
            if(temporary == mkeywords[l] && word == false)
            {

                if (temporary == mkeywords[0] || temporary == mkeywords[9])
                {
                    syntax[iteration_syntax][0] = "program"; // при нахождении данного слова, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mkeywords[1] || temporary == mkeywords[10])
                {
                    syntax[iteration_syntax][0] = "var"; // при нахождении данного слова, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mkeywords[2] || temporary == mkeywords[11])
                {
                    syntax[iteration_syntax][0] = "integer"; // при нахождении данного слова, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mkeywords[3] || temporary == mkeywords[12])
                {
                    syntax[iteration_syntax][0] = "begin"; // при нахождении данного слова, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mkeywords[4] || temporary == mkeywords[13])
                {
                    syntax[iteration_syntax][0] = "if"; // при нахождении данного слова, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mkeywords[5] || temporary == mkeywords[14])
                {
                    syntax[iteration_syntax][0] = "then"; // при нахождении данного слова, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mkeywords[6] || temporary == mkeywords[15])
                {
                    syntax[iteration_syntax][0] = "else"; // при нахождении данного слова, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mkeywords[8] || temporary == mkeywords[17])
                {
                    syntax[iteration_syntax][0] = "end."; // при нахождении данного слова, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mkeywords[7] || temporary == mkeywords[16])
                {
                    syntax[iteration_syntax][0] = "end"; // при нахождении данного слова, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                cout<< "(" << mkeywords[l] << ")" << "      \t Key word"<< endl; // вывод найденного слова
                temporary = ""; // обнуляем временную переменную
                word = true; // говорим, что слово найдено для того что-бы проверка не пошла дальше
            }
        }
    }

    if (word == false)
    {
        for (int l = 0; l<9; l++)
        {
            if (temporary == mlogical_operators[l] && word == false)
            {
                if (temporary == mlogical_operators[0])
                {
                    syntax[iteration_syntax][0] = "<"; // при нахождении данного логического оператора, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mlogical_operators[1])
                {
                    syntax[iteration_syntax][0] = ">"; // при нахождении данного логического оператора, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mlogical_operators[2])
                {
                    syntax[iteration_syntax][0] = "<="; // при нахождении данного логического оператора, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mlogical_operators[3])
                {
                    syntax[iteration_syntax][0] = ">="; // при нахождении данного логического оператора, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mlogical_operators[4])
                {
                    syntax[iteration_syntax][0] = "="; // при нахождении данного логического оператора, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mlogical_operators[5])
                {
                    syntax[iteration_syntax][0] = "or"; // при нахождении данного логического оператора, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mlogical_operators[6])
                {
                    syntax[iteration_syntax][0] = "and"; // при нахождении данного логического оператора, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mlogical_operators[8])
                {
                    syntax[iteration_syntax][0] = "xor"; // при нахождении данного логического оператора, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mlogical_operators [7])
                {
                    syntax[iteration_syntax][0] = "<>"; // при нахождении данного логического оператора, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                cout<< "(" << mlogical_operators[l] << ")"<< "\t\t Logical operator"<< endl; // вывод найденного логического оператора
                temporary = ""; // обнуляем временную переменную
                word = true; // говорим, что слово найдено для того что-бы проверка не пошла дальше
            }
        }
    }
    if (word == false)
    {
        for (int l = 0; l<5; l++)
        {
            if (temporary == marithmetic_operators[l] && word == false)
            {
                if (temporary == marithmetic_operators[0])
                {
                    syntax[iteration_syntax][0] = "+"; // при нахождении данного математического оператора, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == marithmetic_operators[1])
                {
                    syntax[iteration_syntax][0] = ":="; // при нахождении данного математического оператора, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == marithmetic_operators[2])
                {
                    syntax[iteration_syntax][0] = "*"; // при нахождении данного математического оператора, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == marithmetic_operators[3])
                {
                    syntax[iteration_syntax][0] = "/"; // при нахождении данного математического оператора, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == marithmetic_operators[4])
                {
                    syntax[iteration_syntax][0] = "-"; // при нахождении данного математического оператора, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                cout<< "(" << marithmetic_operators[l] << ")"<< "\t\t Mathematical operator" << endl; // вывод найденного математического оператора
                temporary = ""; // обнуляем временную переменную
                word = true; // говорим, что слово найдено для того что-бы проверка не пошла дальше
            }
        }
    }
    if (word == false)
    {
        for (int l = 0; l<6; l++)
        {
            if (temporary == mspecial_symbols[l] && word == false)
            {
                if (temporary == mspecial_symbols[0])
                {
                    syntax[iteration_syntax][0] = ","; // при нахождении данного специального символа, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mspecial_symbols[1])
                {
                    syntax[iteration_syntax][0] = ";"; // при нахождении данного специального символа, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mspecial_symbols[2])
                {
                    syntax[iteration_syntax][0] = "("; // при нахождении данного специального символа, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mspecial_symbols[3])
                {
                    syntax[iteration_syntax][0] = ")"; // при нахождении данного специального символа, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mspecial_symbols[4])
                {
                    syntax[iteration_syntax][0] = "'"; // при нахождении данного специального символа, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                if (temporary == mspecial_symbols[5])
                {
                    syntax[iteration_syntax][0] = ":"; // при нахождении данного специального символа, записываем его в массив синтаксиса
                    iteration_syntax++;
                }
                cout<< "(" <<mspecial_symbols[l] << ")" << "\t\t Special character" << endl; // вывод найденного специального символа
                temporary = ""; // обнуляем временную переменную
                word = true; // говорим, что слово найдено для того что-бы проверка не пошла дальше
            }
        }
    }
}

void synx() // функция синтаксиса
{
    int sk = 0; // переменная для подсчёта открытых и закрытых скобок
    if(syntax[0][0] != "program") // если первый записанный элемент в массив не является program выводим ошибку
    {
        error_message(1, syntax[0][0]);
    }
    if(syntax[sl][0] == "program") // если program первый элемент
    {
        sl++;
        if(syntax[sl][0] != "APP") error_message(2, syntax[sl][0]); // то проверяем есть ли после него название программы
        obivlenie[nums] = syntax[sl][1];
        nums++;
        ass(syntax[sl][1],1);
        sl++;
        if(syntax[sl][0] != ";") error_message(3, syntax[sl][0]); // если название программы найденно, то проверяем стоит ли после него ;
        sl++;
        if(syntax[sl][0] != "var") error_message(4, syntax[sl][0]); // проверяем что стоит поле ;, если не var то ошибка
    }
    if(syntax[sl][0] == "var") // если var то проверяем обявляются ли далее переменные
    {
m1:     sl++;
        if(syntax[sl][0] != "APP") error_message(5, syntax[sl][0]);
        obivlenie[nums] = syntax[sl][1]; // все переменные которые обявлены записываются в массив обявленных переменных
        nums++;
        ass(syntax[sl][1],2);
        sl++;
        if(syntax[sl][0] == ",")
        {
            goto m1; // если найдена запитая после переменной, то переходим в начало для просмотра обявления переменных
        }
        for (int i=0; i<=nums; i++) // проверка на две одинаковые переменные из объявленных
        {
            for (int j=0; j<=nums;j++)
            {
                if (obivlenie[i] == obivlenie[j] && i!=j)
                {
                    error_message(21,obivlenie[i]);
                }
            }
        }
        for (int i=0; i<nums; i++) // проверка объявленных переменных
        {
            for (int j=0; j<digit; j++)
            {
                if (obivlenie[i] == mnumbers[j])
                {
                    error_message(8,obivlenie[i]); // если объявленная переменная является числом, тогда выдаём ошибку
                }
            }
        }
        if(syntax[sl][0] != ":") error_message(6, syntax[sl][0]); // стоит-ли двоеточие
        sl++;
        if(syntax[sl][0] != "integer") error_message(7, syntax[sl][0]); // объявлен-ли тип
        sl++;
        if(syntax[sl][0] != ";") error_message(3, syntax[sl][0]); // обнаружена-ли ;
    }
    sl++;
    if (syntax[sl][0] == "begin") // проверка на то, что после var идёт тело программы
    {
        sl++; // далее идут проверки на то что может стоять после begin - это может быть либо присвоение переменной, либо оператор if, либо сразу конец программы
m4:     if (syntax[sl][0] == "APP") // если стоит переменная, то после неё может быть, только присвоение
        {
            mathtriegger = 0;
            published(syntax[sl][1]); // проверка на обявленность переменной
            if(wrd == 1) error_message(8, syntax[sl][1]);  // если переменная является числом то выдаём ошибку
            sl++;
            if (syntax[sl][0] != ":=") error_message(9, syntax[sl][0]);
            sl++;
            write_sl[0] = sl;
m2:         if (syntax[sl][0] == "APP") // после присвоения может стоять либо переменная, либо открывающая скобка
            {
                published(syntax[sl][1]); // проверка на обявленность переменной, на данном этапе переменная может быть числом
                if(wrd == 1)
                {
                    ass(syntax[sl][1],3);
                    ass(syntax[sl-2][1],4);

                }
                sl++; // рассматриваем, вырожение на все возможные операции
                if (syntax[sl][0] == "APP") error_message(13, syntax[sl][0]);
m3:                 if (syntax[sl][0] == "+" || syntax[sl][0] == "-" || syntax[sl][0] == "/" || syntax[sl][0] == "*")
                {
                    sl++;
                    goto m2;
                }
                if (syntax[sl][0] == ")")
                {
                    sk--;
                    skobka_counter++;
                    sl++;
                    goto m3;
                }
                if (syntax[sl][0] == ";")
                {
                    write_sl[1] = sl;
                    if (((write_sl[1]-write_sl[0])-skobka_counter) != 1)
                    {
                        ass("0",5);
                    }
                    skobka_counter = 0;
                    sl++;
                    goto m4;
                }
                error_message(14, syntax[sl][0]);
            }
            if (syntax[sl][0] == "(")
            {
                sk++;
                skobka_counter++;
                sl++;
                goto m2;
            }
        }
        if (syntax[sl][0] == "if") // если найден оператор if
        {
            sl++;
            write_sl[0] = sl;
s1:         if (syntax[sl][0] == "APP") // если после if найден оператор
            {
                published(syntax[sl][1]);
                sl++;
s12:            if (syntax[sl][0] == "<" || syntax[sl][0] == ">" || syntax[sl][0] == "<=" || syntax[sl][0] == ">=" || syntax[sl][0] == "=" ||syntax[sl][0] == "<>")
                {
                    sl++; // если был найден логический оператор проходим все варинты которые возможны после него
s2:                 if (syntax[sl][0] == "APP") // возможная переменная
                    {
                        published(syntax[sl][1]);
                        sl++;
s3:                     if (syntax[sl][0] == "+" || syntax[sl][0] == "-" || syntax[sl][0] == "/" || syntax[sl][0] == "*") // возможное математическое вырожение
                        {
                            sl++;
                            if (syntax[sl][0] == "(")
                            {
                                sl++;
                                skobka_counter++;
                                sk++;
                            }
                            goto s2;
                        }
                        if (syntax[sl][0] == "then") // возможный оператор then
                        {
                            write_sl[1] = sl;
                            if (((write_sl[1]-write_sl[0])-skobka_counter) != 1)
                            {
                                ass("0",6);
                            }
                            sl++; // расмотрение всех возможных действий после оператора then
                            if (syntax[sl][0] == "if")
                            {
                                sl++;
                                goto s1;
                            }
                            if (syntax[sl][0] == "APP") // если найдено вырожение, или переприсвоение одной переменной другой
                            {
s6:                             published(syntax[sl][1]);
                                if(wrd == 1) error_message(8, syntax[sl][1]);
                                sl++;
                                if (syntax[sl][0] != ":=") error_message(9, syntax[sl][0]);
                                sl++;
                                write_sl[0] = sl;
s4:                             if (syntax[sl][0] == "APP")
                                {
                                    published(syntax[sl][1]);
                                    if(wrd == 1)
                                    {
                                        ass(syntax[sl][1],3);
                                        ass(syntax[sl-2][1],4);

                                    }
                                    sl++;
s5:                                 if (syntax[sl][0] == "+" || syntax[sl][0] == "-" || syntax[sl][0] == "/" || syntax[sl][0] == "*")
                                    {
                                        sl++;
                                        if (syntax[sl][0] == "(")
                                        {
                                            sl++;
                                            sk++;
                                            skobka_counter++;
                                        }
                                        goto s4;
                                    }
                                    if (syntax[sl][0] == ")")
                                    {
                                        sl++;
                                        sk--;
                                        skobka_counter++;
                                        goto s5;
                                    }
                                    if (syntax[sl][0] == ";")
                                    {
                                        write_sl[1] = sl;
                                        ass("0",5);
                                        skobka_counter = 0;
                                        sl++;
s11:                                    if (syntax[sl][0] == "else") // если был найден оператор else, то просматриваем все возможные варианты
                                        {
                                            sl++;
                                            if (syntax[sl][0] == "if")
                                            {
                                                sl++;
                                                goto s1;
                                            }
                                            if (syntax[sl][0] == "begin")
                                            {
                                                goto s7;
                                            }
                                            if (syntax[sl][0] == "APP")
                                            {
                                                goto s6;
                                            }
                                            error_message(18,syntax[sl][0]);
                                        }
                                        if (syntax[sl][0] == "APP")
                                        {
                                            goto s6;
                                        }
                                        if (syntax[sl][0] == "end.") // если найдено окончание программы, то переходим в конец проверки синтаксиса
                                        {
                                            ass("0",7);
                                            sl++;
                                            if (syntax[sl][0] != "\0") error_message(20, syntax[sl][0]); // если после конца программы есть ещё что-то, то выдаём ошибку
                                            sl--;
                                            goto end;
                                        }
                                        if (syntax[sl][0] == "if")
                                        {
                                            sl++;
                                            goto s1;
                                        }
                                        if (syntax[sl][0] == "end") // если найдено окончание операторных скобок, проверяем что стоит за ними
                                        {
                                            sl++;
                                            if (syntax[sl][0] == ";")
                                            {
                                                begincheck--;
                                                sl++;
                                                goto s11;
                                            }
                                        }
                                        error_message(12,syntax[sl][0]);
                                    }
                                    error_message(12,syntax[sl][0]);
                                }
                                if (syntax[sl][0] == "(")
                                {
                                    sl++;
                                    sk++;
                                    goto s4;
                                }
                                error_message(12, syntax[sl][0]);
                            }
s7:                         if (syntax[sl][0] == "begin") // если после then найдены операторные скобки
                            {
                                begincheck++; // увеличиваем значение счётчика операторных скобок на 1
                                sl++; // проверяем все возможные варианты событий после нахождения операторных скобок
                                if (syntax[sl][0] == "APP") // если найдено вырожение, или переприсвоение одной переменной другой
                                {
s10:                                published(syntax[sl][1]);
                                    if(wrd == 1) error_message(8, syntax[sl][1]);
                                    sl++;
                                    if (syntax[sl][0] != ":=") error_message(9, syntax[sl][0]);
                                    sl++;
s8:                                 if (syntax[sl][0] == "APP")
                                    {
                                        published(syntax[sl][1]);
                                        sl++;
s9:                                     if (syntax[sl][0] == "+" || syntax[sl][0] == "-" || syntax[sl][0] == "/" || syntax[sl][0] == "*")
                                        {
                                            sl++;
                                            if (syntax[sl][0] == "(")
                                            {
                                                sl++;
                                                sk++;
                                            }
                                            goto s8;
                                        }
                                        if (syntax[sl][0] == ")")
                                        {
                                            sl++;
                                            sk--;
                                            goto s9;
                                        }
                                        if (syntax[sl][0] == ";")
                                        {
                                            sl++;
                                            if (syntax[sl][0] == "APP")
                                            {
                                                goto s10;
                                            }
                                            if (syntax[sl][0] == "if")
                                            {
                                                sl++;
                                                goto s1;
                                            }
                                            if (syntax[sl][0] == "end") // если найдено окончание операторных скобок, проверяем что стоит за ними
                                            {
                                                sl++;
                                                if (syntax[sl][0] == ";")
                                                {
                                                    begincheck--;
                                                    sl++;
                                                    goto s11;
                                                }
                                                error_message(3, syntax[sl][0]);
                                            }
                                            if (syntax[sl][0] == "begin")
                                            {
                                                sl++;
                                                goto s7;
                                            }
                                            error_message(12,syntax[sl][0]);
                                        }
                                        error_message(12,syntax[sl][0]);
                                    }
                                    if (syntax[sl][0] == "(")
                                    {
                                        sl++;
                                        sk++;
                                        goto s8;
                                    }
                                    error_message(12,syntax[sl][0]);
                                }
                                if (syntax[sl][0] == "if")
                                {
                                    sl++;
                                    goto s1;
                                }
                                if (syntax[sl][0] == "end") // если найдено окончание операторных скобок, проверяем что стоит за ними
                                {
                                    sl++;
                                    if (syntax[sl][0] == ";")
                                    {
                                        begincheck--;
                                        sl++;
                                        goto s11;
                                    }
                                    error_message(3, syntax[sl][0]);
                                }
                                if (syntax[sl][0] == "end.") // если найдено окончание программы, то переходим в конец проверки синтаксиса
                                {
                                    sl++;
                                    if (syntax[sl][0] != "\0") error_message(20, syntax[sl][0]); // если после конца программы есть ещё что-то, то выдаём ошибку
                                    sl--;
                                    goto end;
                                }
                            }
                            error_message(12,syntax[sl][0]);
                        }
                        if (syntax[sl][0] == "and" || syntax[sl][0] == "or" || syntax[sl][0] == "xor") // возможные дополнительые логические операторы
                        {
                            sl++; // проверка на то,что дополнительный логический оператор стоит между двумя логисекими вырожениями и отделён скобками
                            if (syntax[sl][0] == "(")
                            {
                                sl-=2;
                                if (syntax[sl][0] == ")")
                                {
                                    sl+=2;
                                    goto s1;
                                }
                            }
                            error_message(22,syntax[sl][0]);
                        }
                        if (syntax[sl][0] == ")")
                        {
                            sl++;
                            skobka_counter++;
                            sk--;
                            goto s3;
                        }
                        error_message(12,syntax[sl][0]);
                    }
                    if (syntax[sl][0] == "(")
                    {
                        sl++;
                        skobka_counter++;
                        sk++;
                        goto s2;
                    }
                    error_message(9,syntax[sl][0]);
                }
                if (syntax[sl][0] == "+" || syntax[sl][0] == "-" || syntax[sl][0] == "/" || syntax[sl][0] == "*") // если после оператора найдены математические символы, проходим по всем вариантам вырожений
                {
                    sl++;
                    if (syntax[sl][0] == "(")
                    {
                        sl++;
                        sk++;
                    }
                    goto s1;
                }
                if (syntax[sl][0] == ")")
                {
                    sl++;
                    sk--;
                    goto s12;
                }
                error_message(18, syntax[sl][0]);
            }
            if (syntax[sl][0] == "(")
            {
                sk++;
                sl++;
                goto s1;
            }
            error_message(12, syntax[sl][0]);
        }
        if (syntax[sl][0] == "end.")
        {
            sl++;
            if (syntax[sl][0] != "\0") error_message(20, syntax[sl][0]); // если после конца программы есть ещё что-то, то выдаём ошибку
            sl--;
            goto end;
        }
        error_message(12, syntax[sl][0]);
    }
end:    if (sk != 0) error_message(10, syntax[sl][0]); // если счётчик круглых скобок не равен нулю то выдаём ошибку, так как при таком случае бдет существовать незакрытая скобка
        if (begincheck != 0) error_message(10, syntax[sl][0]); // если счётчик операторных скобок не закрыт, то выдаём ошибку так как при таком случае существует незакрытая операторная скобка
}

void published(string num) // функция проверки на обявленность переменной
{
    wrd = 0;
    for (int i=0; i<digit; i++)
    {
        if (num == mnumbers[i]) // если переданное слово в функцию совподает с массивом чисел ставим флаг wrd = 1
        {
            wrd = 1;
        }
    }
    if (wrd == 0)
    {
        for(int i = 0; i<nums; i++)
        {
            if(num == obivlenie[i]) // если переданное слово в функцию совподает с массивом объявленных переменных
            {
                wrd = 2; // ставим флаг wrd = 2;
            }
        }
    }
    if(wrd == 0) error_message(11,syntax[sl][1]); // если переданное слово не является числом и не является обявленным выводим ошибку
}

void error_message(int error, string num) // функция вывода ошибок
{
    setlocale(LC_ALL, "Russian"); // подключение русского языка в консоли
    ifstream fin("errors.err", std::ios::in); // открытие файла где написаны коды ошибок с пояснениями
    string string_errors; // временная строка в которую будет загружаться строка из файла с нужной ошибкой
    int number_errors = 1; // указатель с какой строки начинать чтение файла
    int kol_errors = 24; // колличество строк с ошибками
    while(number_errors < kol_errors) // цикл вывода нужной ошибки
    {
        getline(fin, string_errors);
        if (number_errors == error) cout << string_errors << " (" << num << ")" << endl;
        number_errors++;
    }
    fin.close(); // закрытие файла
    exit(0); // прирыванеи выполнения программы
}

void ass(string asss, int asik) // функция генератора кода
{
    ofstream file;
    file.open("asm.txt", ios::app);
    switch (asik)
    {
    case 1:
    {
        cout << endl << asss << " start 0" << endl;
        file << endl << asss << " start 0" << endl;
        break;
    }
    case 2:
    {
        cout << asss << " word 0" << endl;
        file << asss << " word 0" << endl;
        break;
    }
    case 3:
    {
        cpp++;
        cout <<"p"<<cpp<< " word " << asss << endl;
        cout << "   lda " << "p" << cpp << endl;
        file <<"p"<<cpp<< " word " << asss << endl;
        file << "   lda " << "p" << cpp << endl;
        break;
    }
    case 4:
    {
        cout << "   sta " << asss << endl;
        file << "   sta " << asss << endl;
        break;
    }
    case 5:
    {
        int ssmc = 0, ssmccount = 0,sklogic = 0,skcount = 0,numbsk = 1,check = 0,ll = 0,tag = 0; // наши математические переменные определяющие вершины скобок, их количство, так же наши нумерации переменных
        bool load = false; bool logicon = false; bool oncheck = false;// логическая составляющая представляет собой булианы которые отвечают за то что функция выполнена или что раздел
        for (int i=write_sl[0]; i<write_sl[1]; i++)//определенный цикл от начала присвоения до конца исключая само присовение и точку с запятой
        {
            if(syntax[i][0] == "(") // если мы находим открывающуюся скобку то обнуляем значения скобок вершин numbsk и numbsk2
            {
                ssmccount++;
                ssmc ++;
                numbsk = 1;
                skcount = 0;
            }
            if(syntax[i][0] == ")") // если мы находим закрывающую скобу то считаем ее вершиной и считаем от нее
            {
                check = 1;
                skcount++;
                if(skcount == numbsk) // если вершина открывающей скокби совпадает с закрыващей скобкой
                {
                    int lock = i;
                    for(lock = i-1; lock > write_sl[0]; lock --)
                    {
                        //cout << "lock " << lock << endl;
                        if(syntax[lock][0] == ")") // если мы находим открывающую скобку то считаем ее вершину
                        {
                            check ++; // считаем количество вершин открывающих скобок, они должны совпадать с numbsk2
                        }
                        if(syntax[lock][0] == "(") // если мы находим открывающую скобку то считаем ее вершину
                        {
                            check --; // считаем количество вершин открывающих скобок, они должны совпадать с numbsk2
                        }
                        if(check == 0) // если вершины совпадают то онуляем и так как в цикле вы вычилали лок то знаем от куда до куда идек скокба т.к. от lock до i
                        {
                            check = 0; // обнуляем посчитанные открытые скокби т.е. вершины
                            break;
                        }
                    }
                    for(int count = lock+1; count < i; count++) // задаем цикл от вершины открывающей скокби до вершины закрывающей скобки т.к. от lock до i минуя начальную и конечную скобку
                    {
                        if(syntax[count][0] == "(" && mathtriegger == 0) // если мы встретили открывающую скобку и мат символ не найден то значит это бывшая вершина и пока она не будет закрыта математические действия в ней не выполняются
                        {
                            sklogic++;
                        }
                        if(syntax[count][0] == ")" && mathtriegger == 0)// если мы встретили открывающую скобку и мат символ не найден то значит это бывшая вершина и пока она не будет закрыта математические действия в ней не выполняются
                        {
                            sklogic --;
                        }
                        if(sklogic == 0) // проверка на то что вершины закрыты это значит мы находимся в ашей вершине  и скобки в нутри не учитывались
                        {
                            if(syntax[count][0] == "APP") // если мы нашли переменную
                            {
                                if(mathtriegger == 0)// математический триггер ранее не найден т.е в данном случае * или /
                                {
                                    if(logicon == false) // функция не закончила свои действия
                                    {
                                            cout << "   lda " << syntax[count][1] << endl; // загружаем эту переменную
                                            file << "   lda " << syntax[count][1] << endl;
                                            load = true;
                                    }
                                }
                            }
asmb1:                      if(mathtriegger > 0) // если ранее был найден математический триггер
                            {
                                switch(mathtriegger)// переключаем по нему
                                {
                                case 3:// нашли умножение
                                {
                                    if(syntax[count][0] == "(")// если метка в данном случае находится на скобке это значит что внутри скобок присутвуют еще скокби
                                    {
                                        if(load == false)// ранее загрузки переменной не произвондилсь т.е. lda не вызывалась
                                        {
                                            tag++;
                                            cout << "   lda " << "p"<< semlogic[tag] << endl;// вызывает загрузку нашей переменной которая была этого
                                            file << "   lda " << "p"<< semlogic[tag] << endl;
                                            tag++;
                                            cout << "   mul " << "p" << semlogic[tag] << endl;// и умножаем ее на метку внутренней скобки
                                            file << "   mul " << "p" << semlogic[tag] << endl;
                                            sklogic++;

                                        }
                                        if(load == true)// если ранее производилась загрузка переменной
                                        {
                                            tag++;
                                            cout << "   mul " << "p" << semlogic[tag] << endl;// умножаем ее на метку внутренней скобки
                                            file << "   mul " << "p" << semlogic[tag] << endl;
                                            sklogic++;
                                            load = false;// обнуляем загрузку переменной
                                        }
                                    }
                                    else
                                    {
                                        if(syntax[count+1][0] != "(" && syntax[count-1][0] != ")")// если мы умножаем не на скокби
                                        {
                                            cout << "   mul " << syntax[count][1] << endl;// умножаем переменную в уравнении на загруженную ранее
                                            file << "   mul " << syntax[count][1] << endl;
                                            if(syntax[count+1][0] != "+" && syntax[count-1][0] != "-")// проверяем нет ли между на ми плюса и минуса
                                            {
                                            if(syntax[count+1][0] == "*") mathtriegger = 3;//проверяем следующий элемент на триггер
                                            if(syntax[count+1][0] == "/") mathtriegger = 4;//проверяем следующий элемент на триггер
                                            count +=2;//перепрыгиваем на 2 числа вперед чтобы пропустить умножение и узнать что дальше
                                            goto asmb1;
                                            }
                                        }
                                    }
                                    mathtriegger = 0;
                                    break;
                                }
                                case 4:
                                {
                                    if(syntax[count][0] == "(")// если метка в данном случае находится на скобке это значит что внутри скобок присутвуют еще скокби
                                    {
                                        if(load == false)// ранее загрузки переменной не произвондилсь т.е. lda не вызывалась
                                        {
                                            tag++;//переключаем счетчик переменной
                                            cout << "   lda " << "p"<< semlogic[tag] << endl;// загружаемнашу переменную
                                            file << "   lda " << "p"<< semlogic[tag] << endl;
                                            tag++;//переключаем счетчик переменной
                                            cout << "   div " << "p" << semlogic[tag] << endl;// делим переменную на переменную из уравнения
                                            file << "   div " << "p" << semlogic[tag] << endl;
                                            sklogic++;
                                        }
                                        if(load == true)// ранее загрузки переменной произвондилсь т.е. lda вызывалась
                                        {

                                            tag++;//переключаем счетчик переменной
                                            cout << "   div " << "p" << semlogic[tag] << endl;// делим переменную в уравнении на загруженную ранее
                                            file << "   div " << "p" << semlogic[tag] << endl;
                                            sklogic++;
                                            load = false;
                                        }
                                    }
                                    else
                                    {
                                        cout << "   div " << syntax[count][1] << endl;
                                        file << "   div " << syntax[count][1] << endl;
                                        if(syntax[count+1][0] != "(" && syntax[count+1][0] != ")")
                                        {
                                            if(syntax[count+1][0] != "+" && syntax[count+1][0] != "-")
                                            {
                                                if(syntax[count+1][0] == "*") mathtriegger = 3;
                                                if(syntax[count+1][0] == "/") mathtriegger = 4;
                                                count +=2;
                                                goto asmb1;
                                            }
                                        }
                                    }
                                    mathtriegger = 0;
                                    break;
                                }
                                }
                                logicon = true; // включается сохранение числа
                                if(logicon == true) // если требуется сохранение числа
                                {
                                cpp++;
                                cout <<"p" <<cpp<< " word 0" << endl;
                                file <<"p" <<cpp<< " word 0" << asss << endl;
                                cout << "   sta " "p"<< cpp << endl;
                                file << "   sta " "p"<< cpp << endl;
                                ll++;
                                semlogic[ll] = cpp;
                                logicon = false;
                                // идет сохранение числа
                                }
                            }
                            //==============================================
                            if(syntax[count][0] == "*") mathtriegger = 3;
                            if(syntax[count][0] == "/") mathtriegger = 4;
                            //==============================================
                        }
                    }
                    // хочу немного пояснить, выше обсалютно другой подхой, подход ниже похож на БНФ что для меня явилось на удивление проще.
                    for(int count = lock+1; count < i; count++) // задаем цикл от вершины открывающей скокби до вершины закрывающей скобки т.к. от lock до i минуя начальную и конечную скобку
                    {
                        if(syntax[count][0] == "(")
                        {
                            sklogic++;
                        }
                        if(syntax[count][0] == ")")
                        {
                            sklogic --;
                        }
                        if(sklogic == 0)// если мы на ходимся в нашей скокобке
                        {
                            if(syntax[count][0] == "+") // если мы нашли плюс
                            {
                                if((syntax[count-2][0] == "*" ||syntax[count-2][0] == "/") && (syntax[count+2][0] == "*" ||syntax[count+2][0] == "/")) // проверяем что за 2 символа до
                                //и после былы высокоприоритетные операций и так же проверяем что уравнение не закончилось
                                {
                                    tag++;//переключаем счетч кпеременной
                                    cout << "   lda " << "p"<< semlogic[tag] << endl;//загружаем нашу переменную
                                    file << "   lda " << "p"<< semlogic[tag] << endl;
                                    tag++;
                                    cout << "   add " << "p" << semlogic[tag] << endl;//складываем с операцией после а именно с переменной после
                                    file << "   add " << "p" << semlogic[tag] << endl;
                                    logicon = true;
                                }
                            }
                            if(syntax[count][0] == "-")
                            {
                                if((syntax[count-2][0] == "*" ||syntax[count-2][0] == "/") && (syntax[count+2][0] == "*" ||syntax[count+2][0] == "/"))// проверяем что за 2 символа до
                                //и после былы высокоприоритетные операций и так же проверяем что уравнение не закончилось
                                {
                                    cout << "   lda " << "p"<< semlogic[tag] << endl;//загружаем нашу переменную
                                    file << "   lda " << "p"<< semlogic[tag] << endl;
                                    tag++;
                                    cout << "   sub " << "p" << semlogic[tag] << endl;//вычтаем с операцией после а именно с переменной после
                                    file << "   sub " << "p" << semlogic[tag] << endl;
                                    logicon = true;
                                }
                            }
                        }
                    }
                    if(logicon == true)//если уравнение за кончилось то сохраняем переменную
                    {
                    cpp++;
                    cout <<"p" <<cpp<< " word 0" << endl;
                    file <<"p" <<cpp<< " word 0" << asss << endl;
                    cout << "   sta " "p"<< cpp << endl;
                    file << "   sta " "p"<< cpp << endl;
                    ll++;
                    semlogic[ll] = cpp;
                    logicon = false;
                    }
                    ssmc --;
                    numbsk++;
                    }
                }
            }
        bool mylogic = true;//булиан моей логики нужен чтобы присекать вхождение цикла в другие условия, если он true значит условие выполнялось ранее и он не войдет в другие условия на
        //этой метке в этом месте
        for (int count=write_sl[0]; count<write_sl[1]; count++)//объявляем цикл опять же от присвоение до точки с запятой
        {
            if(syntax[count][0] == "(")
            {
                sklogic++;
            }
            if(syntax[count][0] == ")")
            {
                sklogic --;
            }
            if(sklogic == 0)// те же вершинные скобки, если они ровны 0 то мы работаем в своей вершине
            {
                if(oncheck == true && (syntax[count][0] == "+" || syntax[count][0] == "-" || count+1 == write_sl[1]))//делаем проверку если мы ранее выполняли умножение ил вычитание то
                    //onchek true потом проверяем дошли ли мы до плюса или минуса если до знаем что умножение закончилось и так же проверяем что уравнение не закончилось
                {
                     cpp++;
                     cout <<"p" <<cpp<< " word 0" << endl;
                     file <<"p" <<cpp<< " word 0" << asss << endl;
                     cout << "   sta " "p"<< cpp << endl;
                     file << "   sta " "p"<< cpp << endl;
                     ll++;
                     semlogic[ll] = cpp;
                     oncheck = false;
                     //производим сохранение
                }
                if(syntax[count][0] == "*" || syntax[count][0] == "/")//если мы нашли умножение или деление
                {
                    if(syntax[count-1][0] == "(" && syntax[count+1][0] == ")")//если мы делим или умножаем на скобки, в зависимости от умножения или деления делаем дейсвтия ниже
                    {

                        cout << "   lda " << "p"<< semlogic[tag] << endl;
                        file << "   lda " << "p"<< semlogic[tag] << endl;
                        tag++;
                        if(syntax[count][0] == "/")
                        {
                            cout << "   div " << "p" << semlogic[tag] << endl;
                            file << "   div " << "p" << semlogic[tag] << endl;
                        }
                        if(syntax[count][0] == "*")
                        {
                            cout << "   mul " << "p" << semlogic[tag] << endl;
                            file << "   mul " << "p" << semlogic[tag] << endl;
                        }
                        oncheck = true;
                        mylogic = true;
                    }
                    if(syntax[count-1][0] == ")")// если мы нашли скобку после на 2 символа позже
                    {

                         tag++;
                         cout << "   lda " << "p"<< semlogic[tag] << endl;//загружаем все данные в скобке и в завимисости от данных делим или умножаем
                         file << "   lda " << "p"<< semlogic[tag] << endl;
                         if(syntax[count][0] == "*")
                         {
                             cout << "   mul " << syntax[count+1][1] << endl;
                             file << "   mul " << syntax[count+1][1] << endl;
                         }
                         if(syntax[count][0] == "/")
                         {
                             cout << "   div " << syntax[count+1][1] << endl;
                             file << "   div " << syntax[count+1][1] << endl;
                         }
                         oncheck = true;
                         mylogic = true;
                    }
                    if(syntax[count+1][0] == "(")//если мы нашли скокбу на 2 символа раньше
                    {

                         cout << "   lda " << syntax[count][1] << endl;//загружаем нашу переменную из уравнения и в зависимости от оператора делим или умножаем на скокбу
                         file << "   lda " << syntax[count][1] << endl;
                         if(syntax[count][0] == "*")
                         {
                             cout << "   mul " << "p"<< semlogic[tag] << endl;
                             file << "   mul " << "p"<< semlogic[tag] << endl;
                         }
                         if(syntax[count][0] == "/")
                         {
                             cout << "   div " << "p"<< semlogic[tag] << endl;
                             file << "   div " << "p"<< semlogic[tag] << endl;
                         }
                         tag++;
                         oncheck = true;
                         mylogic = true;
                    }
                    if(syntax[count+1][0] == "APP" && mylogic == false)//если мы нашли нашу переменную в уравнении но не нашли ничего из предложенного
                    {
                        if(oncheck == true)//если мы ранее уже умножали переменные в заисимости от оператора делим или умножаем напеременную из уравнения
                        {
                            if(syntax[count][0] == "*")
                            {
                                cout << "   mul " << syntax[count+1][1] << endl;
                                file << "   mul " << syntax[count+1][1] << endl;
                            }
                            if(syntax[count][0] == "/")
                            {
                                cout << "   div " << syntax[count+1][1] << endl;
                                file << "   div " << syntax[count+1][1] << endl;
                            }
                        }
                        else
                        {
                            if(syntax[count-1][0] == "APP")//если мы нашли переменную в уравнении позднее
                            {
                                if((syntax[count-2][0] == "+" || syntax[count-2][0] == "-" || syntax[count-2][0] == ")") && (syntax[count+2][0] == "+" || syntax[count+2][0] == "-" || syntax[count+2][0] == "(" || count+2 == write_sl[1]))
                                {
                                    //проверяем если ранее  или позднее был плюс или скобка и проверяем если позднее был найден плюс или скобка
                                    cout << "   lda " << syntax[count-1][1] << endl;//загружаем нашу переменную из уравнения и в зависимости от данных делим или умножаем на следующую переменную
                                    file << "   lda " << syntax[count-1][1] << endl;
                                    if(syntax[count][0] == "*")
                                    {
                                        cout << "   mul " << syntax[count+1][1] << endl;//
                                        file << "   mul " << syntax[count+1][1] << endl;
                                    }
                                    if(syntax[count][0] == "/")
                                    {
                                        cout << "   div " << syntax[count+1][1] << endl;
                                        file << "   div " << syntax[count+1][1] << endl;
                                    }
                                    oncheck = true;
                                }
                            }
                        }
                    }
                }
                mylogic = false;
            }
        }
        for (int count=write_sl[0]; count<write_sl[1]; count++)
        {
            if(syntax[count][0] == "(")
            {
                sklogic++;
            }
            if(syntax[count][0] == ")")
            {
                sklogic --;
            }
            if(sklogic == 0)//опять же проверка скобок вершин
            {
                if(((oncheck == true) && (syntax[count][0] == "*" && syntax[count][0] == "/")) || count++ == write_sl[1]) //проверяем если мы уже ранее считали
                    //если мы нашли умножение или же деление и не закончилось ли на уравнение
                {
                    cpp++;
                    cout <<"p" <<cpp<< " word 0" << endl;
                    file <<"p" <<cpp<< " word 0" << asss << endl;
                    cout << "   sta " "p"<< cpp << endl;
                    file << "   sta " "p"<< cpp << endl;
                    ll++;
                    semlogic[ll] = cpp;
                    oncheck = false;
                }
                if(syntax[count][0] == "+" || syntax[count][0] == "-")// если мы нашли + или -
                {
                    if((syntax[count+2][0] == "/" || syntax[count+2][0] == "*") && (syntax[count-2][0] == "*" || syntax[count-2][0] == "/"))// если мы нашли первым умножение и деление и вторым тоже
                    {
                        if(oncheck == true)//если мы уже ранее складывали
                        {
                            tag++;
                            if(syntax[count][0] == "+")//если нашли плюс
                            {
                                cout << "   add " <<"p"<< semlogic[tag] << endl;
                                file << "   add " <<"p"<< semlogic[tag] << endl;
                            }
                            if(syntax[count][0] == "-")//если нашли минус
                            {
                                cout << "   sub " <<"p"<< semlogic[tag] << endl;
                                file << "   sub " <<"p"<< semlogic[tag] << endl;
                            }
                        }
                        else// если не складывали ранее
                        {

                            cout << "   lda " << "p"<< semlogic[tag] << endl;//загржаем нашу переменную
                            file << "   lda " << "p"<< semlogic[tag] << endl;
                            tag++;
                            if(syntax[count][0] == "+")
                            {
                                cout << "   add " <<"p"<< semlogic[tag] << endl;//складываем ее с другой переменной
                                file << "   add " <<"p"<< semlogic[tag] << endl;
                            }
                            if(syntax[count][0] == "-")
                            {
                                cout << "   sub " <<"p"<< semlogic[tag] << endl;
                                file << "   sub " <<"p"<< semlogic[tag] << endl;
                            }
                        }
                        goto asmb3; //чтобы не выполнялись другие действия сразу переходим в конец цикла
                        oncheck = true;
                    }
                    if(syntax[count-1][0] == "(" && syntax[count+1][0] == ")") //если сы складываем скокби
                    {
                        cout << "   lda " << "p"<< semlogic[tag] << endl;//загружаем переменную из скобок 1
                        file << "   lda " << "p"<< semlogic[tag] << endl;
                        tag++;
                        if(syntax[count][0] == "+")
                        {
                        cout << "   add " << semlogic[tag] << endl;//загружаем вторую переменную из скобок
                        file << "   add " << semlogic[tag] << endl;
                        }
                        if(syntax[count][0] == "-")
                        {
                        cout << "   sub " << semlogic[tag] << endl;
                        file << "   sub " << semlogic[tag] << endl;
                        }
                        oncheck = true;
                    }
                    if(syntax[count+1][0] == "(")//если ранее мы нашли скокбу
                    {
                        cout << "   lda " << "p"<< semlogic[tag] << endl;//загружаем нашу переменную
                        file << "   lda " << "p"<< semlogic[tag] << endl;
                        tag++;
                        if(syntax[count][0] == "+")
                        {
                        cout << "   add " << syntax[count-1][1] << endl;//складываем
                        file << "   add " << syntax[count-1][1] << endl;
                        }
                        if(syntax[count][0] == "-")
                        {
                        cout << "   sub " << syntax[count-1][1] << endl;//вычитаем
                        file << "   sub " << syntax[count-1][1] << endl;
                        }
                        oncheck = true;
                    }
                    if(syntax[count-1][0] == ")")//если мы нашли скобку закрытия
                    {
                        cout << "   lda " << "p"<< semlogic[tag] << endl;//загржаем переменную
                        file << "   lda " << "p"<< semlogic[tag] << endl;
                        tag++;
                        if(syntax[count][0] == "+")
                        {
                        cout << "   add " << syntax[count+1][1] << endl;//складываем
                        file << "   add " << syntax[count+1][1] << endl;
                        }
                        if(syntax[count][0] == "-")
                        {
                        cout << "   sub " << syntax[count+1][1] << endl;
                        file << "   sub " << syntax[count+1][1] << endl;
                        }
                        oncheck = true;
                    }
                    if(syntax[count+1][0] == "APP" && syntax[count-1][0] == "APP")//если мы складываем 2 переменные
                    {
                        if(oncheck == true)//если ранее еще складывали
                        {
                            if(syntax[count][0] == "+")
                            {
                                cout << "   add " << syntax[count+1][1] << endl;//складываем
                                file << "   add " << syntax[count+1][1] << endl;
                            }
                            if(syntax[count][0] == "-")
                            {
                                cout << "   sub " << syntax[count+1][1] << endl;//вычитаем
                                file << "   sub " << syntax[count+1][1] << endl;
                            }
                        }
                        else//если ранее не складывали
                        {
                            if(syntax[count][0] == "+")
                            {
                            cout << "   lda " << syntax[count-1][1] << endl;//загружаем переменную
                            file << "   lda " << syntax[count-1][1] << endl;
                            cout << "   add " << syntax[count+1][1] << endl;//складываем
                            file << "   add " << syntax[count+1][1] << endl;
                            oncheck = true;
                            }
                            if(syntax[count][0] == "-")
                            {
                            cout << "   lda " << syntax[count-1][1] << endl;//загружаем переменную
                            file << "   lda " << syntax[count-1][1] << endl;
                            cout << "   sub " << syntax[count+1][1] << endl;//вычитаем
                            file << "   sub " << syntax[count+1][1] << endl;
                            oncheck = true;
                            }
                        }
                        tag++;
                    }
                }
asmb3:          mylogic = false;
            }
        }
        mathtriegger = 0;
        cout << "   sta " << syntax[write_sl[0]-2][1] << endl;
        file << "   sta " << syntax[write_sl[0]-2][1] << endl;
        break;
    }
    case 6:
    {
        mathtriegger = 0;
        int one_yr = write_sl[0];
        for (int i = write_sl[0]; i<write_sl[1];i++)
        {
            if (syntax[i][0] == ">" || syntax[i][0] == "<"|| syntax[i][0] == ">="|| syntax[i][0] == "<="||syntax[i][0] == "<>" || syntax[i][0] == "=" || syntax[i][0] == "or"||syntax[i][0] == "and")//если нашли лог операцию
            {
                break;
            }
            one_yr++;//находим середину лог уровнения указатель на лог операцию
        }
        for (int i=write_sl[0]; i<one_yr; i++)
        {
            if(syntax[i][0] == "+") mathtriegger = 1;
            if(syntax[i][0] == "-") mathtriegger = 2;
            if(syntax[i][0] == "*") mathtriegger = 3;
            if(syntax[i][0] == "/") mathtriegger = 4;
            if (syntax[i][0] == "APP")//если нашли операнд
            {
                if(mathtriegger == 0)//если ранее ненашли оператор
                {
                cout << "   lda " << syntax[i][1] << endl;//загружаем операнд
                file << "   lda " << syntax[i][1] << endl;
                }
                if(mathtriegger > 0)//если мы нашли операнд
                {
                    switch(mathtriegger)
                    {
                    case 1:{
                        cout << "   add " << syntax[i][1]<< endl;//складываем
                        file << "   add " << syntax[i][1] << endl;
                        mathtriegger = 0;
                        break;

                    }
                    case 2:{
                        cout << "   sub " << syntax[i][1] << endl;//вычитаем
                        file << "   sub " << syntax[i][1] << endl;
                        mathtriegger = 0;
                        break;
                    }
                    case 3:{
                        cout << "   mul " << syntax[i][1] << endl;//умножаем
                        file << "   mul " << syntax[i][1] << endl;
                        mathtriegger = 0;
                        break;
                    }
                    case 4:{
                        cout << "   div " << syntax[i][1] << endl;//делим
                        file << "   div " << syntax[i][1] << endl;
                        mathtriegger = 0;
                        break;
                    }
                    }
                }
            }
         }
        //сохраняем переменную
        mathtriegger = 0;
        cpp++;
        cout <<"p"<<cpp<< " word 0"  << endl;
        cout << "   sta " << "p"<< cpp << endl;
        file <<"p"<<cpp<< " word 0"  << endl;
        file << "   sta " << "p"<< cpp << endl;
        //те же действия для второй половины if
        for (int i=one_yr+2; i<write_sl[1]; i++)
        {
            if(syntax[i][0] == "+") mathtriegger = 1;
            if(syntax[i][0] == "-") mathtriegger = 2;
            if(syntax[i][0] == "*") mathtriegger = 3;
            if(syntax[i][0] == "/") mathtriegger = 4;
            if (syntax[i][0] == "APP")
            {
                if(mathtriegger == 0)
                {
                    cout << "   lda " << syntax[i][1] << endl;
                    file << "   lda " << syntax[i][1] << endl;
                }
                if(mathtriegger > 0)
                {
                    switch(mathtriegger)
                    {
                    case 1:{
                        cout << "   add " << syntax[i][1]<< endl;
                        file << "   add " << syntax[i][1] << endl;
                        mathtriegger = 0;
                        break;

                    }
                    case 2:{
                        cout << "   sub " << syntax[i][1] << endl;
                        file << "   sub " << syntax[i][1] << endl;
                        mathtriegger = 0;
                        break;
                    }
                    case 3:{
                        cout << "   mul " << syntax[i][1] << endl;
                        file << "   mul " << syntax[i][1] << endl;
                        mathtriegger = 0;
                        break;
                    }
                    case 4:{
                        cout << "   div " << syntax[i][1] << endl;
                        file << "   div " << syntax[i][1] << endl;
                        mathtriegger = 0;
                        break;
                    }
                    }
                }
            }
        }
        mathtriegger = 0;
        cpp++;
        metka+=1;
        //сохраняем переменную
        cout <<"p"<<cpp<< " word 0" << endl;
        cout << "   sta " << "p"<< cpp << endl;
        file <<"p"<<cpp<< " word 0" << endl;
        file << "   sta " << "p"<< cpp << endl;
        //ивходя из лог оператора выполняем действия и ставим метку
        if (syntax[one_yr][0] == "<")
        {
            cout << "   comp " << "p" <<cpp-1 << endl;
            cout << "   jgt fin" << metka << endl;
            file << "   comp " << "p" <<cpp-1 << endl;
            file << "   jgt fin" << metka << endl;
        }
        if (syntax[one_yr][0] == ">")
        {
            cout << "   comp " << "p" <<cpp-1 << endl;
            cout << "   jlt fin" << metka << endl;
            file << "   comp " << "p" <<cpp-1 << endl;
            file << "   jlt fin" << metka << endl;
        }
        if (syntax[one_yr][0] == "<=")
        {
            cout << "   comp " << "p" <<cpp-1 << endl;
            cout << "   jgt fin" << metka << endl;
            file << "   comp " << "p" <<cpp-1 << endl;
            file << "   jgt fin" << metka << endl;
        }
        if (syntax[one_yr][0] == ">=")
        {
            cout << "   comp " << "p" <<cpp-1 << endl;
            cout << "   jlt fin" << metka << endl;
            file << "   comp " << "p" <<cpp-1 << endl;
            file << "   jlt fin" << metka << endl;
        }
        if (syntax[one_yr][0] == "<>")
        {
            cout << "   comp " << "p" <<cpp-1 << endl;
            cout << "   jeq fin" << metka << endl;
            file << "   comp " << "p" <<cpp-1 << endl;
            file << "   jeq fin" << metka << endl;
        }
        if (syntax[one_yr][0] == "=")
        {
            cout << "   comp " << "p" <<cpp-1 << endl;
            cout << "   jeq fin" << metka << endl;
            file << "   comp " << "p" <<cpp-1 << endl;
            file << "   jeq fin" << metka << endl;
        }
        break;
    }
    case 7:
    {
        cout <<"fin"<< metka << "   hlt" << endl;//если нашли конец программы то ставим туда метку выхода
        cout <<"   end " << syntax[1][1] << endl;
        file <<"fin"<< metka << "   hlt" << endl;
        file <<"   end " << syntax[1][1] << endl;
    }
    }
    file.close();
}
