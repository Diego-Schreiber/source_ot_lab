#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std;

// Una pieza conserva sus propias caracteristicas.
struct Pieza {
    string nombre;
    string color;
};

// Estructura responsable de administrar la lista de componentes.
class ListaPiezas {
private:
    vector<Pieza> piezas_;

public:
    void agregarOActualizar(const string& nombre, const string& color) {
        for (auto& pieza : piezas_) {
            if (pieza.nombre == nombre) {
                pieza.color = color;
                return;
            }
        }
        piezas_.push_back({nombre, color});
    }

    const vector<Pieza>& obtenerTodas() const { return piezas_; }
};

// Producto1 del diagrama.
class Automovil {
public:
    string modelo;
    string motorizacion;
    string colorCarroceria;
    string pais;
    double precio = 0.0;
    ListaPiezas piezas;

    void mostrar() const {
        cout << "\n========== AUTOMOVIL CONFIGURADO ==========\n"
             << "Modelo: " << modelo << '\n'
             << "Motorizacion: " << motorizacion << '\n'
             << "Color de carroceria: " << colorCarroceria << '\n'
             << "Pais: " << pais << '\n'
             << "Precio elegido: S/ " << fixed << setprecision(2) << precio << '\n'
             << "Componentes seleccionados:\n";
        if (piezas.obtenerTodas().empty()) {
            cout << "  (ninguno)\n";
        } else {
            for (const auto& pieza : piezas.obtenerTodas()) {
                cout << "  - " << pieza.nombre << " | Color: " << pieza.color << '\n';
            }
        }
        cout << "==========================================\n";
    }
};

// Opciones solicitadas por el cliente. La lista tambien es una clase.
struct Configuracion {
    string modelo;
    string motorizacion;
    string colorCarroceria;
    string pais;
    double precio = 0.0;
    ListaPiezas piezas;
};

// IBuilder del diagrama.
class IBuilder {
public:
    virtual ~IBuilder() = default;
    virtual void reiniciar() = 0;
    virtual void establecerModelo(const string&) = 0;
    virtual void establecerMotorizacion(const string&) = 0;
    virtual void establecerColorCarroceria(const string&) = 0;
    virtual void establecerPais(const string&) = 0;
    virtual void establecerPrecio(double) = 0;
    virtual void agregarPieza(const Pieza&) = 0;
    virtual Automovil obtenerProducto() = 0;
};

// Builder Especifico del diagrama.
class BuilderEspecifico final : public IBuilder {
private:
    Automovil producto_;

public:
    void reiniciar() override { producto_ = Automovil{}; }
    void establecerModelo(const string& valor) override { producto_.modelo = valor; }
    void establecerMotorizacion(const string& valor) override { producto_.motorizacion = valor; }
    void establecerColorCarroceria(const string& valor) override { producto_.colorCarroceria = valor; }
    void establecerPais(const string& valor) override { producto_.pais = valor; }
    void establecerPrecio(double valor) override { producto_.precio = valor; }
    void agregarPieza(const Pieza& pieza) override {
        producto_.piezas.agregarOActualizar(pieza.nombre, pieza.color);
    }
    Automovil obtenerProducto() override { return std::move(producto_); }
};

// Director del diagrama: coordina los pasos de construccion.
class Director {
public:
    Automovil construir(IBuilder& builder, const Configuracion& configuracion) const {
        builder.reiniciar();
        builder.establecerModelo(configuracion.modelo);
        builder.establecerMotorizacion(configuracion.motorizacion);
        builder.establecerColorCarroceria(configuracion.colorCarroceria);
        builder.establecerPais(configuracion.pais);
        builder.establecerPrecio(configuracion.precio);
        for (const auto& pieza : configuracion.piezas.obtenerTodas()) {
            builder.agregarPieza(pieza);
        }
        return builder.obtenerProducto();
    }
};

string leerTexto(const string& mensaje) {
    string valor;
    do {
        cout << mensaje;
        getline(cin, valor);
        if (!cin) throw runtime_error("Error al leer la entrada.");
        if (valor.empty()) cout << "El valor no puede estar vacio.\n";
    } while (valor.empty());
    return valor;
}

int leerEntero(const string& mensaje, int minimo, int maximo) {
    while (true) {
        string entrada = leerTexto(mensaje);
        try {
            size_t procesados = 0;
            int numero = stoi(entrada, &procesados);
            if (procesados == entrada.size() && numero >= minimo && numero <= maximo)
                return numero;
        } catch (const exception&) {}
        cout << "Ingrese un numero entre " << minimo << " y " << maximo << ".\n";
    }
}

double leerPrecio() {
    while (true) {
        string entrada = leerTexto("Precio deseado (S/): ");
        try {
            size_t procesados = 0;
            double valor = stod(entrada, &procesados);
            if (procesados == entrada.size() && valor > 0 && valor <= 1'000'000'000)
                return valor;
        } catch (const exception&) {}
        cout << "Ingrese un precio positivo y valido (use punto para decimales).\n";
    }
}

string elegir(const string& titulo, const vector<string>& opciones) {
    cout << "\n" << titulo << '\n';
    for (size_t i = 0; i < opciones.size(); ++i) {
        cout << i + 1 << ". " << opciones[i] << '\n';
    }
    int indice = leerEntero("Opcion: ", 1, static_cast<int>(opciones.size()));
    return opciones[static_cast<size_t>(indice - 1)];
}

// Cliente del diagrama: selecciona las caracteristicas.
class Cliente {
public:
    Configuracion solicitarConfiguracion() const {
        Configuracion config;
        config.modelo = elegir("Seleccione modelo:", {"Sedan", "SUV", "Hatchback", "Pickup"});
        config.motorizacion = elegir("Seleccione motorizacion:",
                                     {"Gasolina", "Diesel", "Hibrido", "Electrico"});
        config.colorCarroceria = leerTexto("Color de la carroceria: ");
        config.pais = elegir("Seleccione pais de destino:", {"Peru", "Chile", "Colombia", "Mexico"});
        config.precio = leerPrecio();

        const vector<string> catalogo = {
            "Puertas", "Llantas", "Timon", "Asientos",
            "Motor", "Espejos", "Vidrios", "Parachoques", "Faros"
        };

        cout << "\nSeleccione las piezas que desea incluir.\n";
        while (true) {
            cout << "\n0. Terminar seleccion de piezas\n";
            for (size_t i = 0; i < catalogo.size(); ++i)
                cout << i + 1 << ". " << catalogo[i] << '\n';
            int opcion = leerEntero("Pieza: ", 0, static_cast<int>(catalogo.size()));
            if (opcion == 0) break;

            const string& nombre = catalogo[static_cast<size_t>(opcion - 1)];
            string color = leerTexto("Color para " + nombre + ": ");
            config.piezas.agregarOActualizar(nombre, color);
            cout << "Pieza registrada (si ya existia, se actualizo su color).\n";
        }
        return config;
    }
};

int main() {
    try {
        Cliente cliente;
        Director director;
        BuilderEspecifico builder;

        cout << "=== PERSONALIZADOR DE AUTOMOVILES - BUILDER ===\n";
        Configuracion configuracion = cliente.solicitarConfiguracion();
        Automovil automovil = director.construir(builder, configuracion);
        automovil.mostrar();
    } catch (const exception& error) {
        cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
