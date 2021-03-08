#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stack>
#include <set>
using namespace std;

struct Node
{
    int x;
    int y;
    int rodzicX;
    int rodzicY;
    float heurestykaH;
    float kosztG;
    float sumaF;
};

inline bool operator < (const Node& lhs, const Node& rhs)
{
    return lhs.sumaF < rhs.sumaF;
}
ostream& operator<<(ostream& os, const Node& node)
{
    os << '(' << node.x << ',' << node.y << ')';
    return os;
}

vector<Node> AGwiazdka(Node, Node, double**);
bool czyMozeByc(int, int); //Sprawdzamy czy współrzędne nie wychodzą poza zakres lub nie są przeszkodą
bool czyToCel(int, int, Node);  //Sprawdzamy czy to cel
float obliczH(int, int, Node);  //Obliczamy odległość Euklidesową
vector<Node> stwórzDrogę(Node [21][21], Node, double**); //Wyznaczenie trasy do celu
bool czyToPrzeszkoda(double**, int, int); //Sprawdzamy czy to przeszkoda

int main()
{ 
    cout << "Wczytywanie danych z pliku\n";

    string nazwap = "grid.txt";
    int wym2 = 20;
    int wym1 = 20;

    //teraz deklarujemy dynamicznie tablice do, której wczytamy nasz plik,
    int rows = wym2 + 1;
    double** G;
    G = new double* [rows];
    while (rows--) G[rows] = new double[wym1 + 1];

    cout << "\n\nNacisnij ENTER aby wczytac tablice o nazwie " << nazwap;
    getchar();

    std::ifstream plik(nazwap.c_str());

    for (unsigned int i = 1; i < wym2 + 1; i++)
    {
        for (unsigned int j = 1; j < wym1 + 1; j++)
        {
            plik >> G[i][j];
        }
    }
    plik.close();
    
    Node cel;
    cel.x = 1;
    cel.y = 20;
    
    Node start;
    start.x = 20;
    start.y = 1;
    G[start.x][start.y] = 3;
    vector<Node> wypisz = AGwiazdka(start, cel, G);
    for (vector<Node>::const_iterator iterator = wypisz.begin(); iterator != wypisz.end(); ++iterator)
    {
        cout << *iterator << ' ';        
    }
    cout << "\nWypisujemy na ekran\n\n";
    for (int i = 1; i < wym2 + 1; i++)
    {
        for (int j = 1; j < wym1 + 1; j++)
        {
            cout << " " << G[i][j];
        }cout << "\n";
    }
    //na koniec czyścimy pamięć po naszej tablicy
    for (int i = 0; i < wym2 + 1; i++)
    {
        delete[] G[i];
    }//czyscimy wiersze
    delete[] G;//zwalniamy tablice wskaznikow do wierszy
    cout << "\n\nNacisnij ENTER aby zakonczyc";
    getchar();

    return 0;
}
vector<Node> AGwiazdka(Node start, Node cel, double** G)
{
    //Warunki startowe   
    vector<Node> pusty;
    if (czyMozeByc(cel.x, cel.y) == false)
    {
        cout << "Cel wykracza poza zakres siatki" << endl;
        return pusty;
    }    
    if (czyToPrzeszkoda(G, cel.x, cel.y) == true)
    {
        cout << "Cel jest przeszkodą" << endl;
        return pusty;
    }
    if (czyToCel(start.x, start.y, cel) == true)
    {
        cout << "Start jest celem." << endl;
        return pusty;
    }  
    vector<Node> listaOtwarta; //Lista otwarta typu set
    bool listaZamknięta[21][21]; //Lista zamknięta typu bool
    Node mapa[21][21]; //Mapa przechowywanych wartości
    for (int i = 1; i < 21; i++)
    {
        for (int j = 1;  j < 21;  j++) //Ustawianie wartości mapy na wartości ogólne
        {            
            mapa[i][j].kosztG = FLT_MAX;
            mapa[i][j].sumaF = FLT_MAX;
            mapa[i][j].heurestykaH = FLT_MAX;
            mapa[i][j].rodzicX = -1;
            mapa[i][j].rodzicY = -1;
            mapa[i][j].x = i;
            mapa[i][j].y = j;

            listaZamknięta[i][j] = false;   
        }
    }
    int i = start.x; //Przypisanie współrzędnych startu do zmiennych
    int j = start.y;
    mapa[i][j].sumaF = 0.0;
    mapa[i][j].kosztG = 0.0;
    mapa[i][j].heurestykaH = 0.0; //Ustawienie wartości punktu startowego
    mapa[i][j].rodzicX = i;
    mapa[i][j].rodzicY = j;

    listaOtwarta.emplace_back(mapa[i][j]); //Dodanie punktu startowego do listy otwartej
    bool czyZnaleziono = false;  //Informacja czy odnaleziono cel

    while (!listaOtwarta.empty())
    {                     
        float kosztG2;
        float heurestykaH2; //Pomocniczne wartości 
        float sumaF2;
        Node n;
        do
        {
            float pomoc = FLT_MAX;
            vector<Node>::iterator iteratorNode;
            for (vector<Node>::iterator iterator = listaOtwarta.end(); iterator != listaOtwarta.begin();)
            {
                --iterator;
                Node n = *iterator;
                if (n.sumaF < pomoc)    //Określenie pozycji Node'a o najmniejszej wartości f z listy otwartej;
                {
                    pomoc = n.sumaF;
                    iteratorNode = iterator;
                }
            }
            n = *iteratorNode;
            listaOtwarta.erase(iteratorNode); //Usuwanie Node'a o najmniejszej wartości f z listy otwartej;
        } while (czyMozeByc(n.x, n.y) == false);
        i = n.x;
        j = n.y;

        listaZamknięta[i][j] = true;  //Ustawienie wartości true w zamkniętej liście o współrzędnych Node'a wyciągniętego z listy otwartej
        if (czyMozeByc(i - 1, j) == true)
        {
            if (czyToCel(i - 1, j, cel) == true)
            {             
                mapa[i - 1][j].rodzicX = i;
                mapa[i - 1][j].rodzicY = j;
                cout << "Znaleziono cel ";
                  
                czyZnaleziono = true;
                return stwórzDrogę(mapa, cel, G);  //Stworzenie drogi celu 
            }
            else if(listaZamknięta[i-1][j] == false && czyToPrzeszkoda(G, i-1,j) == false)
            {
                kosztG2 = n.kosztG + 1.0;
                heurestykaH2 = obliczH(i - 1, j, cel); //Obliczenie heurestyki dla danych współrzędnych
                sumaF2 = kosztG2 + heurestykaH2;
                if (mapa[i - 1][j].sumaF == FLT_MAX || mapa[i - 1][j].sumaF > sumaF2)
                {
                    mapa[i - 1][j].sumaF = sumaF2;
                    mapa[i - 1][j].kosztG = kosztG2;
                    mapa[i - 1][j].heurestykaH = heurestykaH2; //Ustawienie na mapie nowych wartości dla danych współrzędnych
                    mapa[i - 1][j].rodzicX = i;
                    mapa[i - 1][j].rodzicY = j;
                    listaOtwarta.emplace_back(mapa[i - 1][j]); //Dodanie wartości na mapie do listy otwartej
                }
            }
        }
        if (czyMozeByc(i + 1, j) == true)
        {
            if (czyToCel(i + 1, j, cel) == true)
            {
                mapa[i + 1][j].rodzicX = i;
                mapa[i + 1][j].rodzicY = j;
                cout << "Znaleziono cel ";
                  
                czyZnaleziono = true;
                return stwórzDrogę(mapa, cel, G); //Stworzenie drogi celu 
            }
            else if (listaZamknięta[i + 1][j] == false && czyToPrzeszkoda(G, i + 1, j) == false)
            {
                kosztG2 = n.kosztG + 1.0;
                heurestykaH2 = obliczH(i + 1, j, cel); //Obliczenie heurestyki dla danych współrzędnych
                sumaF2 = kosztG2 + heurestykaH2;
                if (mapa[i + 1][j].sumaF == FLT_MAX || mapa[i + 1][j].sumaF > sumaF2)
                {               
                    mapa[i + 1][j].sumaF = sumaF2;
                    mapa[i + 1][j].kosztG = kosztG2;
                    mapa[i + 1][j].heurestykaH = heurestykaH2; //Ustawienie na mapie nowych wartości dla danych współrzędnych
                    mapa[i + 1][j].rodzicX = i;
                    mapa[i + 1][j].rodzicY = j;
                    listaOtwarta.emplace_back(mapa[i + 1][j]); //Dodanie wartości na mapie do listy otwartej
                }
            }
        }
        if (czyMozeByc(i, j + 1) == true)
        {
            if (czyToCel(i, j + 1, cel) == true)
            {
                mapa[i][j + 1].rodzicX = i;
                mapa[i][j + 1].rodzicY = j;
                cout << "Znaleziono cel ";
                 
                czyZnaleziono = true;
                return stwórzDrogę(mapa, cel, G); //Stworzenie drogi celu  
            }
            else if (listaZamknięta[i][j + 1] == false && czyToPrzeszkoda(G, i, j + 1) == false)
            {
                kosztG2 = n.kosztG + 1.0;
                heurestykaH2 = obliczH(i, j + 1, cel); //Obliczenie heurestyki dla danych współrzędnych
                sumaF2 = kosztG2 + heurestykaH2;
                if (mapa[i][j + 1].sumaF == FLT_MAX || mapa[i][j + 1].sumaF > sumaF2)
                {
                    mapa[i][j + 1].sumaF = sumaF2;
                    mapa[i][j + 1].kosztG = kosztG2;
                    mapa[i][j + 1].heurestykaH = heurestykaH2; //Ustawienie na mapie nowych wartości dla danych współrzędnych
                    mapa[i][j + 1].rodzicX = i;
                    mapa[i][j + 1].rodzicY = j;
                    listaOtwarta.emplace_back(mapa[i][j + 1]); //Dodanie wartości na mapie do listy otwartej
                }
            }
        }
        if (czyMozeByc(i, j - 1) == true)
        {
            if (czyToCel(i, j - 1, cel) == true)
            {
                mapa[i][j - 1].rodzicX = i;
                mapa[i][j - 1].rodzicY = j;
                cout << "Znaleziono cel ";
                  
                czyZnaleziono = true;
                return stwórzDrogę(mapa, cel, G); //Stworzenie drogi celu 
            }
            else if (listaZamknięta[i][j - 1] == false && czyToPrzeszkoda(G, i, j - 1) == false)
            {
                kosztG2 = n.kosztG + 1.0;
                heurestykaH2 = obliczH(i, j - 1, cel); //Obliczenie heurestyki dla danych współrzędnych
                sumaF2 = kosztG2 + heurestykaH2;
                if (mapa[i][j - 1].sumaF == FLT_MAX || mapa[i][j - 1].sumaF > sumaF2)
                {                 
                    mapa[i][j - 1].sumaF = sumaF2;
                    mapa[i][j - 1].kosztG = kosztG2;
                    mapa[i][j - 1].heurestykaH = heurestykaH2; //Ustawienie na mapie nowych wartości dla danych współrzędnych
                    mapa[i][j - 1].rodzicX = i;
                    mapa[i][j - 1].rodzicY = j;
                    listaOtwarta.emplace_back(mapa[i][j - 1]); //Dodanie wartości na mapie do listy otwartej
                }
            }
        }
    }
    if (czyZnaleziono == false)
    {
        cout << "Nie znaleziono celu"; //Sprawdzenie warunku czy odnaleziono cel
    }
    return pusty;
} 
bool czyMozeByc(int x, int y)
{ 
    if (x < 1 || x > 20 || y < 1 || y > 20)
    {
        return false;       //Sprawdzenie czy dany punkt na mapie nie wykracza poza siatkę
    }     
    return true;
}
bool czyToPrzeszkoda(double** G, int x, int y)
{
    if (G[x][y] == 5)
    {
        return true; //Sprawdzenie czy dany punkt na mapie nie jest przeszkodą
    }
    return false;
}
bool czyToCel(int x, int y, Node cel)
{
    if (x == cel.x && y == cel.y)
    {
        return true;    //Sprawdzenie czy dany punkt na mapie nie jest celem
    }
    return false;  
}
float obliczH(int x , int y, Node cel)
{
    float H = sqrt(((x - cel.x) * (x - cel.x) + (y - cel.y) * (y - cel.y)));   //Obliczanie heurestyki dla danego punktu
    return H;
}
vector<Node> stwórzDrogę(Node mapa[21][21], Node cel, double** G)
{
    cout << "Wyznaczam trase do celu" << endl;
    int x = cel.x;
    int y = cel.y;
    stack<Node> stos;
    vector<Node> trasaKońcowa;
    while (!(mapa[x][y].rodzicX == x && mapa[x][y].rodzicY == y))
    {
        stos.push(mapa[x][y]);
        G[x][y] = 3;
        int pomocX = mapa[x][y].rodzicX;
        int pomocY = mapa[x][y].rodzicY;    //Tworzenie drogi do celu z przekazanych współrzędnych
        x = pomocX;
        y = pomocY;
    }
    stos.push(mapa[x][y]);
    while (!stos.empty())
    {
        Node najwyżej = stos.top();
        stos.pop();   
        trasaKońcowa.emplace_back(najwyżej);
    }
    return trasaKońcowa;
}