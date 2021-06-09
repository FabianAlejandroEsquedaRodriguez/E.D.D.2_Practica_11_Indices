#include <iostream>
#include <cstdio>
#include <string.h>
#include <cstdlib>
#include <fstream>
#include <iomanip>

using namespace std;

struct stRegistro {
   char valido;  // Campo que indica si el registro es valido S->Válido, N->Inválido
   char nombre[34];
   char apellido[2][34];
   char telefono[11];
};

struct stIndice {
   char telefono[11];
   long indice;//Campo de registro
};

//Prototipos de las funciones
void Capturar(stRegistro &reg);
void Insertar(FILE *fa, stRegistro &reg);
void EliminarRetornoLinea(char *cad);
void ReconstruirIndices(FILE *fa);
void Mostrar(stRegistro &reg);
void ListarPorTelefonos(FILE *fa);
void ListarNatural(FILE *fa);
void Leer(FILE *fa, stRegistro &reg, char *telefono);

// Funciones para ordenar el fichero de índices:
void Intercambia(FILE *fa, long iz, long de);
char *LeeCampo(FILE *fa, long n, char *buf);
void QuickSort(FILE *fa, long inicio, long final);


int main(void){
    system("color F0");
    system("cls");

    stRegistro registro;
    
    FILE *fa;
    size_t opc;
    char telefono[11];

    fa = fopen("contactos.dat", "r+b");          // Este modo permite leer y escribir
    if(!fa) fa = fopen("contactos.dat", "w+b");  // si el fichero no existe, lo crea.

    do
    {
        cout<<"\t\tMENU - AGENDA DE CONTACTOS"<<endl<<endl;
        cout<<"(1) INSERTAR CONTACTO"<<endl;
        cout<<"(2) BUSCAR CONTACTO"<<endl;
        cout<<"(3) LISTAR CONTACTOS POR NUMERO DE TELEFONO"<<endl;
        cout<<"(4) LISTAR CONTACTOS POR ORDEN NATURAL"<<endl;
        cout<<"(5) SALIR"<<endl<<endl;
        
        cout<<"DIGITA UNA OPCION: ";
        cin>>opc;
        cin.ignore();

        system("cls");

        switch (opc)
        {
            case 1: cout<<"\tINSERTAR CONTACTO"<<endl<<endl;
                    Capturar(registro);
                    Insertar(fa, registro);
                    Mostrar(registro);

                    cout<<endl<<endl<<endl;
                    system("pause"); system("cls");
                break;
                

            case 2: cout<<"\t\tBUSCAR CONTACTO"<<endl<<endl;
                    do {
                        cout<<"Contacto que deseas buscar (No. de telefono): ";
                        fgets(telefono, 11, stdin);
                        EliminarRetornoLinea(telefono);
                    } while(strlen(telefono) < 1);

                    Leer(fa, registro, telefono);
                    Mostrar(registro);

                    cout<<endl<<endl<<endl;
                    system("pause"); system("cls");

                break;
            
            case 3: cout<<"\t\tLISTAR CONTACTOS POR NUMERO DE TELEFONO"<<endl<<endl;
                    ListarPorTelefonos(fa);

                    cout<<endl<<endl<<endl;
                    system("pause"); system("cls");

                break;

            case 4: cout<<"\t\tLISTAR CONTACTOS POR ORDEN NATURAL"<<endl<<endl;
                    ListarNatural(fa);

                    cout<<endl<<endl<<endl;
                    system("pause"); system("cls");

                break;

            case 5: cout<<"\t\t\tSALIENDO DEL PROGRAMA..."<<endl<<endl<<endl;
                break;
        }

    }while(opc != 5);

    fclose(fa);

    return 0;
}


// Permite que el usuario introduzca un registro por pantalla
void Capturar(stRegistro &reg)
{
    int i;
    //char numero[6];

    // system("cls");
    cout<<"LEER REGISTRO:\n\n";
    reg.valido = 'S';

    cout<<"NOMBRE: ";
    fgets(reg.nombre, 34, stdin);
    EliminarRetornoLinea(reg.nombre);

    cout<<"APELLIDO PATERNO: ";
    fgets(reg.apellido[0], 34, stdin);
    EliminarRetornoLinea(reg.apellido[0]);

    cout<<"APELLIDO MATERNO: ";
    fgets(reg.apellido[1], 34, stdin);
    EliminarRetornoLinea(reg.apellido[1]);

    cout<<"NUMERO DE TELEFONO: ";
    fgets(reg.telefono, 11, stdin);
    EliminarRetornoLinea(reg.telefono);
}

void EliminarRetornoLinea(char *cad)
{
   int i;
   // la función fgets captura el retorno de línea, hay que eliminarlo:
   for(i = strlen(cad)-1; i >= 0 && cad[i] < ' '; i--) cad[i] = 0;
}

//Funcion para insertar el contacto registrado, en el archivo fisico
void Insertar(FILE *fa, stRegistro &reg)
{
   // Insertar al final:
    fseek(fa, 0, SEEK_END);
    fwrite(&reg, sizeof(stRegistro), 1, fa);
    ReconstruirIndices(fa);
}


void ReconstruirIndices(FILE *fa)
{
    long n=0;
    FILE *fi;
    stRegistro reg;
    stIndice ind;

    // Crea el fichero de índices a partir del archivo de datos:
    fi = fopen("indices.dat", "w+b");
    rewind(fa);
    
    while(fread(&reg, sizeof(stRegistro), 1, fa)) {
        strcpy(ind.telefono, reg.telefono);
        ind.indice = n++;
        fwrite(&ind, sizeof(stIndice), 1, fi);
        cout<<"\n";
    }
    // Ordena usando el algoritmo Quicksort:
    QuickSort(fi, 0, n-1);
    fclose(fi);
}


void QuickSort(FILE *fi, long inicio, long final)
{
    long iz, de;
    char mitad[11];
    static char cad[11];

    iz = inicio;
    de = final;
    strcpy(mitad, LeeCampo(fi, (iz+de)/2, cad));
    do {
        while(strcmp(LeeCampo(fi, iz, cad), mitad) < 0 && iz < final) iz++;
        while(strcmp(mitad, LeeCampo(fi, de, cad)) < 0 && de > inicio) de--;
        if(iz < de) Intercambia(fi, iz, de);
        if(iz <= de) {
            iz++;
            de--;
        }
    } while(iz <= de);
    if(inicio < de) QuickSort(fi, inicio, de);
    if(iz < final) QuickSort(fi, iz, final);
}

char *LeeCampo(FILE *fi, long n, char *buf)
{
    stIndice ind;

    fseek(fi, n*sizeof(stIndice), SEEK_SET);
    fread(&ind, sizeof(stIndice), 1, fi);
    strcpy(buf, ind.telefono);

    return buf;
}

void Intercambia(FILE *fi, long iz, long de)
{
   stIndice reg1, reg2;

    fseek(fi, iz*sizeof(stIndice), SEEK_SET);
    fread(&reg1, sizeof(stIndice), 1, fi);
    fseek(fi, de*sizeof(stIndice), SEEK_SET);
    fread(&reg2, sizeof(stIndice), 1, fi);
    fseek(fi, iz*sizeof(stIndice), SEEK_SET);
    fwrite(&reg2, sizeof(stIndice), 1, fi);
    fseek(fi, de*sizeof(stIndice), SEEK_SET);
    fwrite(&reg1, sizeof(stIndice), 1, fi);
}

void Mostrar(stRegistro &reg)
{
    int i;

    if(reg.valido == 'S') {
        cout<<"\n\nNombre: "<<" "<<reg.nombre<<" "<<reg.apellido[0]<<" "<<reg.apellido[1]<<endl;
        cout<<"Numero de telefono: "<<reg.telefono<<endl;
    }

}

void ListarPorTelefonos(FILE *fa)
{
    FILE *fi;
    stIndice ind;
    stRegistro reg;

    fi = fopen("indices.dat", "rb");

    cout<<endl<<left<<setw(25)<<"NOMBRE"<<setw(34)<<"APELLIDO PATERNO"<<setw(34)<<"APELLIDO MATERNO"<<setw(11)<<"TELEFONO"<<endl;
    
    while(fread(&ind, sizeof(stIndice), 1, fi)) {
        fseek(fa, ind.indice*sizeof(stRegistro), SEEK_SET);
        fread(&reg, sizeof(stRegistro), 1, fa);
        cout<<"\n"<<setw(25)<<reg.nombre<<setw(34)<<reg.apellido[0]<<setw(34)<<reg.apellido[1]<<setw(11)<<reg.telefono<<endl;
    }
    fclose(fi);
}

void ListarNatural(FILE *fa)
{
    stRegistro reg;
    rewind(fa);

    cout<<endl<<left<<setw(25)<<"NOMBRE"<<setw(34)<<"APELLIDO PATERNO"<<setw(34)<<"APELLIDO MATERNO"<<setw(11)<<"TELEFONO"<<endl;

    while(fread(&reg, sizeof(stRegistro), 1, fa))
        cout<<"\n"<<setw(25)<<reg.nombre<<setw(34)<<reg.apellido[0]<<setw(34)<<reg.apellido[1]<<setw(11)<<reg.telefono<<endl;
         
}

void Leer(FILE *fa, stRegistro &reg, char *telefono)
{
    FILE *fi;
    stIndice ind;
    long inf, sup, n, nRegs;

    fi = fopen("indices.dat", "rb");
    fseek(fi, 0, SEEK_END);
    nRegs = ftell(fi)/sizeof(stIndice);

    // Búsqueda binaria:
    inf = 0;
    sup = nRegs-1;

    do {
        n = inf+(sup-inf)/2;
        fseek(fi, n*sizeof(stIndice), SEEK_SET);
        fread(&ind, sizeof(stIndice), 1, fi);
        if(strcmp(ind.telefono, telefono) < 0) inf = n+1;
        else sup = n-1;
    } while(inf <= sup && strcmp(ind.telefono, telefono));

    // Si se encontró el teléfono, lee el registro, si no muestra mensaje.
    if(!strcmp(ind.telefono, telefono)) {
        fseek(fa, ind.indice*sizeof(stRegistro), SEEK_SET);
        fread(&reg, sizeof(stRegistro), 1, fa);
    }
    else {
        reg.valido = 'N';
        printf("\n\nRegistro no encontrado\n");
    }
    fclose(fi);
}