//
//  main.cpp
//  evidencia2
//
//  Created by Victoria Marin on 12/11/24.
//

#include <iostream>
#include <algorithm>
#include <vector>
#include <cfloat>
#include <cmath>
#include <iomanip>
#include <unordered_map>
#include <string>
#include <tuple>

using namespace std;

struct Colonia {
    int x, y;
    string name;
    int central;
    Colonia() : x(0), y(0), name(""), central(0) {}
    Colonia(int x, int y, string name, int central) : x(x), y(y), name(name), central(central) {}
};

struct DisjointSets {
    vector<int> parent, rank;
    DisjointSets(int n) : parent(n), rank(n, 0) {
        for (int i = 0; i < n; ++i) parent[i] = i;
    }
    int find(int u);
    void merge(int u, int v); //union de dos conjuntos dependiendo el rank
};

int DisjointSets::find(int u) {
    if (u != parent[u]){
        parent[u] = find(parent[u]);
    }
    return parent[u];
}

void DisjointSets::merge(int u, int v){
    u = parent[u];
    v = find(v);
    if (rank[u] > rank[v]){
        parent[v] = u;
    }
    else{
        parent[u] = v;
    }
    if(rank[u] == rank[v]){
        rank[v]++; //aqui crece
    }
}

double bestDistance = DBL_MAX; //mejor dist para nuevas colonias
string closestColonyName;

//saca la dist euclidiana
double dist(const Colonia &p1, const Colonia &p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

//si encuentra una colonia mas cerca, se la manda la que existe y la futura
void updateClosestColony(const Colonia &existing, const Colonia &future) {
    double d = dist(existing, future);
    if (d < bestDistance) { //si es mejor que la dist maxima se actualiza el nombre y la dist
        bestDistance = d;
        closestColonyName = existing.name;
    }
}

//encuentra la colonia mas cercana
//O(n)
string findClosestColony(const Colonia &future, const vector<Colonia> &colonies) {
    bestDistance = DBL_MAX;
    for (const auto &colony : colonies) { //por cada punto en colonia lo compara con el futuro
        updateClosestColony(colony, future);
    }
    return closestColonyName;
}

//kruskal
pair<int, vector<tuple<string, string, int>>> kruskalMST(int n, vector<tuple<int, int, int>> &edges, const vector<Colonia> &colonies) {
    sort(edges.begin(), edges.end()); // ordenas los edges por costo
    DisjointSets uf(n);
    int totalCost = 0;
    vector<tuple<string, string, int>> mstEdges;

    for (const auto &[cost, u, v] : edges) {
        if (uf.find(u) != uf.find(v)) { //revisa si u y v estan en sets diferentes
            uf.merge(u, v); // Union u and v
            totalCost += cost;
            mstEdges.emplace_back(colonies[u].name, colonies[v].name, cost);

            // cuando tenga todos los ejes
            if (mstEdges.size() == n - 1) break;
        }
    }
    return {totalCost, mstEdges};
}

int main() {
    int n, m, k, q;
    cin >> n >> m >> k >> q;

    vector<Colonia> colonies(n);
    unordered_map<string, int> colonyIndex;

    //lee las colonias existentes
    for (int i = 0; i < n; i++) {
        string name;
        int x, y, central;
        cin >> name >> x >> y >> central;
        colonies[i] = Colonia(x, y, name, central);
        colonyIndex[name] = i;
    }

    // lee las colonias y su costo
    vector<tuple<int, int, int>> edges;
    for (int i = 0; i < m; i++) {
        string uName, vName;
        int cost;
        cin >> uName >> vName >> cost;
        int u = colonyIndex[uName];
        int v = colonyIndex[vName];
        edges.emplace_back(cost, u, v);
    }
    
    for (int i = 0; i < k; i++) {
        string uName, vName;
        cin >> uName >> vName;
        int u = colonyIndex[uName];
        int v = colonyIndex[vName];
        edges.emplace_back(0, u, v); //simula nuevo cableado
    }

    
    // lee las proximas colonias a conectar
    vector<Colonia> futureColonies(q);
    vector<pair<string, string>> connections;
    for (int i = 0; i < q; i++) {
        string name;
        int x, y;
        cin >> name >> x >> y;
        futureColonies[i] = Colonia(x, y, name, 0);

        //por cada colonia nueva encuentra con que colonia es mas cercana
        string closestColony = findClosestColony(futureColonies[i], colonies);
        connections.emplace_back(name, closestColony);
    }

    //outputs
    
    // llama a kruskal
    auto [totalCost, mstEdges] = kruskalMST(n, edges, colonies);

    cout << "1 – Cableado óptimo de nueva conexión." << endl;
    for (const auto &[colony1, colony2, cost] : mstEdges) {
        cout << colony1 << " - " << colony2 << " " << cost << endl;
    }
    cout << "Costo Total: " << totalCost << endl;

    
    cout << "4 – Conexión de nuevas colonias." << endl;
    for (const auto &connection : connections) {
        cout << connection.first << " debe conectarse con " << connection.second << endl;
    }

    return 0;
}


//
//La primer linea llegan n (2 <= n <= 30), m (1 <= m <= 400), k (0 <= k <= m)  y q (1 <= q <= 10), n = cantidad de colonias, m = número de conexiones entre colonias, k = las conexiones con el nuevo cableado, q = futuras nuevas colonias que se desean conectar.
//Luego vienen n lineas con el nombre de la colonia (no contienen espacios), su posición en el plano carteciano y si es o no una central.
//Posteriomente vienen m lineas con las conexiones entre colonias y su costo.
//Posteriormente viene k lineas con las conexiones entre colonias que cuentan con el nuevo cableado
//Por último vienen q lineas con el nombre de la nueva colonia (no contiene espacios) y los puntos cartecianos de las nuevas colonias.

/*
5 8 1 2
LindaVista 200 120 1
Purisima 150 75 0
Tecnologico -50 20 1
Roma -75 50 0
AltaVista -50 40 0
LindaVista Purisima 48
LindaVista Roma 17
Purisima Tecnologico 40
Purisima Roma 50
Purisima AltaVista 80
Tecnologico Roma 55
Tecnologico AltaVista 15
Roma AltaVista 18
Purisima Tecnologico
Independencia 180 -15
Roble 45 68
*/

/*
 -------------------
 1 – Cableado óptimo de nueva conexión.
 Tecnologico - AltaVista 15
 LindaVista - Roma 17
 Roma - AltaVista 18
 Costo Total: 50
 -------------------
 2 – La ruta óptima.
 Purisima - Roma - AltaVista - Tecnologico - Purisima
 La Ruta Óptima tiene un costo total de: 123
 -------------------
 3 – Caminos más cortos entre centrales.
 LindaVista - Roma - AltaVista - Tecnologico (50)
 -------------------
 4 – Conexión de nuevas colonias.
 Independencia debe conectarse con Purisima
 Roble debe conectarse con AltaVista
 -------------------
 */
