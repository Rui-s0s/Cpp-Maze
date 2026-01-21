# 🧩 Generación y Resolución de Laberintos en C++

## 📌 Descripción
Este proyecto es una práctica orientada al aprendizaje de **lenguajes de bajo nivel**, utilizando **C++**, y a la implementación de **algoritmos clásicos de recorrido de grafos** como **DFS**, **BFS** y **A\***.

El objetivo principal es **generar un laberinto**, **resolverlo automáticamente** y **medir el tiempo de ejecución** de ambos procesos, con el fin de analizar el rendimiento y la velocidad que se obtiene al trabajar con un lenguaje de bajo nivel.

---

## 🎯 Objetivos de Aprendizaje
- Profundizar en el uso de **C++**
- Implementar algoritmos de grafos (**DFS, BFS, A\***)
- Comprender estructuras de datos como:
  - Pilas
  - Colas
  - Grafos
- Manejar **matrices** para representar laberintos
- Validar movimientos dentro de un entorno bidimensional
- Medir tiempos de ejecución para comparar rendimiento
- Fortalecer el **pensamiento computacional**

---

## 🧠 Generación del Laberinto
La generación del laberinto se realiza utilizando el algoritmo **Depth-First Search (DFS)**:
- Explora el laberinto en profundidad
- Genera caminos válidos evitando ciclos
- Produce un laberinto completamente conectado
- Se apoya en una **pila implícita (recursión o stack)**

Este método es común para generar laberintos con una única solución y sin regiones aisladas.

---

## 🧭 Resolución del Laberinto
Para resolver el laberinto se utiliza el algoritmo **A\***:
- Evalúa nodos considerando:
  - Costo real desde el inicio
  - Heurística estimada hasta el objetivo
- Garantiza encontrar el camino óptimo


---

## 🗺️ Representación del Laberinto
- El laberinto se representa como una **matriz bidimensional**
- Cada celda puede ser:
  - Pared
  - Camino
- Se validan movimientos según los límites del tablero
- Se interpretan las celdas como un **grafo implícito**
- 
---

## ⏱️ Medición de Rendimiento
El programa incluye funciones para medir:
- Tiempo de generación del laberinto
- Tiempo de resolución del laberinto

Esto permite analizar el rendimiento y evidenciar la eficiencia del uso de **C++ como lenguaje de bajo nivel**.

---

## ▶️ Compilación y Ejecución

### Compilar
```bash
clang++ main.cpp -o main 
