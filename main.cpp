#include <stdio.h>
#include <omp.h>
#include "WorldMap.h"
#include <time.h>

using namespace std;

int main() {

    clock_t tStart = clock();


    WorldMap* worldMap = new WorldMap();

    //losuj zbior P punktow o licznosci N > 10(n+1), n - wymiar zadania
    worldMap->generateRandomPaths(200,0);

    //wyznacz punkt najlepszy i najgorszy
    int best = worldMap->findBestPath();
    int worst = worldMap->findWorstPath();



    int n = 10;

     vector<Town*> srodek(worldMap->Gettowns().size());
     vector<Town*> odbicie(worldMap->Gettowns().size());
     vector<int> sympleks(n+1);
     double x;
     struct drand48_data randBuffer;
     srand48_r(time(NULL), &randBuffer);
     vector<Town*> lepsza(worldMap->Gettowns().size());
     int utknal = 0;
     int poprz_dlugosc = 0;
     int iteracje = 0;
     bool krecsie = true;
     int iii;

    //#pragma omp parallel for private(iii, srodek, odbicie, sympleks, x, randBuffer, lepsza, utknal, poprz_dlugosc, iteracje, krecsie) num_threads(6) shared(worldMap, best, worst)
    for(iii = 0; iii < 10000; ++iii) {
        iteracje++;
        //if(iteracje % 100 == 0) cout<<"\niteracja "<<iteracje<<" watek: "<<omp_get_thread_num ();


        //losuj ze zbioru P zbior n punktow i utworz n+1 wymiarowy sympleks
        //pierwszy wierzcholek - najlepszy punkt
        //# pragma omp atomic
        sympleks[0] = best;
        //#pragma omp parallel for shared(worldMap, sympleks) private(x, randBuffer) num_threads(4)
        for(int i = 1; i < n+1; ++i) {
            drand48_r(&randBuffer, &x);
            x = (int)(x*1000.0)%worldMap->Getpaths().size();
            sympleks[i] = x; //(worldMap->Getpaths())[x]
        }

        //wyznacz srodek sympleksu
        //#pragma omp parallel for shared(worldMap, srodek) num_threads(4)
        for(int i = 0; i < srodek.size();++i) {
            int xSuma = (worldMap->GetbestPath())[i]->GetX();
            int ySuma = (worldMap->GetbestPath())[i]->GetY();
            for(int j = 1; j < n; ++j) {
                xSuma += (worldMap->Getpaths())[sympleks[j]][i]->GetX();
                ySuma += (worldMap->Getpaths())[sympleks[j]][i]->GetY();
            }
            xSuma /= n;
            ySuma /= n;
            //cout<<"\nwatek: "<<omp_get_thread_num ();
            srodek[i] = new Town(xSuma, ySuma, 7+i, "srodek");
        }

        //operacja odbicia ostatniego punktu sympleksu wzgledem srodka sympleksu
        //#pragma omp parallel for shared(worldMap, odbicie, srodek, sympleks) num_threads(4)
        for(int i = 0; i < srodek.size(); ++i) {
            int x = 2*srodek[i]->GetX() - ((worldMap->Getpaths())[sympleks[sympleks.size()-1]])[i]->GetX();
            int y = 2*srodek[i]->GetY() - ((worldMap->Getpaths())[sympleks[sympleks.size()-1]])[i]->GetY();
            Town* t = new Town(x,y,srodek[i]->Getid(),srodek[i]->Getname());
            odbicie[i] = t;
        }

        //czy odbicie spelnia ograniczenia?
        //zaadaptowanie wyliczonej sciezki do istniejacych miast
        lepsza = worldMap->adjustPath(odbicie);

        //czy jest to punkt lepszy od ostatniego?
        if(worldMap->computePathLength(lepsza) < worldMap->computePathLength(worldMap->GetworstPath())) {
            worldMap->changePath(worst, lepsza);
            //wyznacz punkt najlepszy i najgorszy
            best = worldMap->findBestPath();
            worst = worldMap->findWorstPath();
            utknal = 0;
        }
        else {
            utknal++;
        }


        //jesli algorytm utknal ********************************************************
        if(utknal > 100) {
            cout<<"utknal! roznica: "<<worldMap->computePathLength(worldMap->GetworstPath()) - worldMap->computePathLength(worldMap->GetbestPath())<<endl;
            worldMap->showMap();
            worldMap->drawPath(odbicie, cv::Scalar(0,0,255),2);
            worldMap->drawPath(lepsza, cv::Scalar(255,0,0),2);
            worldMap->drawPath(worldMap->GetbestPath(), cv::Scalar(0,255,0),5);
            cout<<"\nwygrala: ";
            for(int i = 0; i < worldMap->Getpaths()[best].size(); ++i) cout<< worldMap->Getpaths()[best][i]->Getname()<<" ";
            cout<<endl;
            cout<<"\n\nCZAS: "<<(double)(clock() - tStart)/CLOCKS_PER_SEC<<" sekund\n\n";
            cv::imshow("mapa", worldMap->worldMap);
            cv::waitKey();
            iii = 10001;
        }
        poprz_dlugosc = worldMap->computePathLength(worldMap->GetworstPath());
        //jesli algorytm utknal ********************************************************


        //znaleziono rozwiazanie
        if(worldMap->computePathLength(worldMap->GetworstPath()) - worldMap->computePathLength(worldMap->GetbestPath()) == 0) {
            cout<<"roznica: "<<worldMap->computePathLength(worldMap->GetworstPath()) - worldMap->computePathLength(worldMap->GetbestPath())<<endl;
            worldMap->showMap();
            worldMap->drawPath(odbicie, cv::Scalar(0,0,255),2);
            worldMap->drawPath(lepsza, cv::Scalar(255,0,0),2);
            worldMap->drawPath(worldMap->GetbestPath(), cv::Scalar(0,255,0),5);
            cout<<"\nwygrala: ";
            for(int i = 0; i < worldMap->Getpaths()[best].size(); ++i) cout<< worldMap->Getpaths()[best][i]->Getname()<<" ";
            cout<<endl;
            cout<<"\n\nCZAS: "<<(double)(clock() - tStart)/CLOCKS_PER_SEC<<" sekund\n\n";
            cv::imshow("mapa", worldMap->worldMap);
            cv::waitKey();
            iii = 10001;
        }
        //worldMap->drawPath(lepsza, cv::Scalar(255,0,0));
        //cv::imshow("mapa", worldMap->worldMap);

        //cv::waitKey();
    }



    //cout<<"\n\ncalosc: "<<iteracje<<endl;






}
