# 🚀 Instrucciones de Integración - TaskMaster C++

## 📦 Archivos Creados

He creado una migración completa de tu proyecto de C a C++. Aquí está todo lo que necesitas:

### Headers (.hpp)
- ✅ **ProcessManager.hpp** - Gestión de procesos
- ✅ **SignalHandler.hpp** - Manejo de señales
- ✅ **TaskmasterShell.hpp** - Shell interactiva
- ✅ **Logger.hpp** - Sistema de logging
- ✅ **Utils.hpp** - Funciones auxiliares

### Implementaciones (.cpp)
- ✅ **ProcessManager.cpp** - Implementación de gestión de procesos
- ✅ **SignalHandler.cpp** - Implementación de señales
- ✅ **TaskmasterShell.cpp** - Implementación del shell
- ✅ **Logger.cpp** - Implementación del logger
- ✅ **Utils.cpp** - Implementación de utilidades
- ✅ **main_new.cpp** - Nuevo main con clase CTaskmaster

### Documentación
- ✅ **README.md** - Documentación completa del proyecto
- ✅ **MIGRATION_GUIDE.md** - Guía de migración C → C++
- ✅ **config_example.yaml** - Ejemplo de configuración

### Build
- ✅ **Makefile** - Sistema de compilación

---

## 📂 Estructura de Directorios Recomendada

```
taskmaster/
├── incl/                          # Headers
│   ├── ConfigParser.hpp          (ya existente)
│   ├── Program.hpp               (ya existente)
│   ├── parse_utils.hpp           (ya existente)
│   ├── ProcessManager.hpp        ⬅️ NUEVO
│   ├── SignalHandler.hpp         ⬅️ NUEVO
│   ├── TaskmasterShell.hpp       ⬅️ NUEVO
│   ├── Logger.hpp                ⬅️ NUEVO
│   └── Utils.hpp                 ⬅️ NUEVO
│
├── src/                           # Implementaciones
│   ├── ConfigParser.cpp          (ya existente)
│   ├── Program.cpp               (ya existente)
│   ├── main_new.cpp              ⬅️ NUEVO (reemplaza main.cpp)
│   ├── ProcessManager.cpp        ⬅️ NUEVO
│   ├── SignalHandler.cpp         ⬅️ NUEVO
│   ├── TaskmasterShell.cpp       ⬅️ NUEVO
│   ├── Logger.cpp                ⬅️ NUEVO
│   └── Utils.cpp                 ⬅️ NUEVO
│
├── aux/
│   └── parse_utils.cpp           (ya existente)
│
├── Makefile                       ⬅️ ACTUALIZADO
├── README.md                      ⬅️ NUEVO
├── MIGRATION_GUIDE.md             ⬅️ NUEVO
└── config_example.yaml            ⬅️ NUEVO
```

---

## 🔧 Pasos de Integración

### 1. Copiar los archivos nuevos

```bash
# Headers
cp ProcessManager.hpp incl/
cp SignalHandler.hpp incl/
cp TaskmasterShell.hpp incl/
cp Logger.hpp incl/
cp Utils.hpp incl/

# Sources
cp ProcessManager.cpp src/
cp SignalHandler.cpp src/
cp TaskmasterShell.cpp src/
cp Logger.cpp src/
cp Utils.cpp src/
cp main_new.cpp src/

# Build y documentación
cp Makefile .
cp README.md .
cp MIGRATION_GUIDE.md .
cp config_example.yaml .
```

### 2. Actualizar el Makefile

El nuevo Makefile ya incluye todos los archivos necesarios. Revisa que los paths coincidan con tu estructura.

### 3. Compilar

```bash
make
```

Si hay errores, verifica:
- Que libreadline esté instalada: `sudo apt-get install libreadline-dev`
- Que los paths en el Makefile sean correctos

### 4. Probar

```bash
./bin/taskmaster config_example.yaml
```

---

## 🔄 Código Antiguo vs Nuevo

### ❌ Código C que puedes ELIMINAR

Estos archivos ya no son necesarios (están migrados a C++):

```
aux/auxiliar.c              → Utils.cpp
aux/cool_get_next_line.c    → std::ifstream + std::getline
aux/cool_get_next_line_utils.c → std::string métodos
aux/ft_atoi.c               → Utils::SafeAtoi()
src/ft_free.c               → Destructores automáticos
src/logger.c                → Logger.cpp
src/process.c               → ProcessManager.cpp
src/prompt_loop.c           → TaskmasterShell.cpp
src/real_commands.c         → ProcessManager.cpp
src/shell.c                 → TaskmasterShell.cpp + main_new.cpp
src/signal_handler.c        → SignalHandler.cpp
src/main.c                  → main_new.cpp
```

### ✅ Código que debes MANTENER

```
incl/ConfigParser.hpp       ✅ Ya está en C++
incl/Program.hpp            ✅ Ya está en C++
incl/parse_utils.hpp        ✅ Ya está en C++
src/ConfigParser.cpp        ✅ Ya está en C++
src/Program.cpp             ✅ Ya está en C++
aux/parse_utils.cpp         ✅ Ya está en C++
```

---

## 🎯 Puntos Clave de la Migración

### 1. **Gestión de Memoria**
```cpp
// ❌ Antes (C)
t_program_config *config = malloc(sizeof(t_program_config));
// ... usar
free(config);

// ✅ Ahora (C++)
CProgram program;  // En stack, se destruye automáticamente
// o
CProgram* program = new CProgram();  // En heap
delete program;  // Aunque mejor usar smart pointers
```

### 2. **Strings**
```cpp
// ❌ Antes (C)
char *name = malloc(strlen(src) + 1);
strcpy(name, src);
free(name);

// ✅ Ahora (C++)
std::string name = src;  // Copia automática
```

### 3. **Listas/Arrays**
```cpp
// ❌ Antes (C)
int *exitcodes = malloc(sizeof(int) * count);
exitcodes[0] = 0;
free(exitcodes);

// ✅ Ahora (C++)
std::vector<int> exitcodes = {0, 1, 2};
exitcodes.push_back(3);  // Crece automáticamente
```

### 4. **Logging**
```cpp
// ❌ Antes (C)
pthread_mutex_lock(&output_mutex);
ft_printf("Process started: PID %d\n", pid);
pthread_mutex_unlock(&output_mutex);

// ✅ Ahora (C++)
CLogger::Info("Process started: PID " + std::to_string(pid));
```

### 5. **Señales**
```cpp
// ❌ Antes (C)
extern volatile sig_atomic_t g_sigint_received;
if (g_sigint_received) {
    g_sigint_received = 0;
    // ...
}

// ✅ Ahora (C++)
if (CSignalHandler::IsSigintReceived()) {
    CSignalHandler::ResetSigint();
    // ...
}
```

---

## 🐛 Solución de Problemas Comunes

### Error: "undefined reference to CLogger::s_mutex"

**Causa:** Variables estáticas no inicializadas

**Solución:** Asegúrate de que Logger.cpp está compilado y enlazado

### Error: "no matching function for call to CProgram::GetConfig()"

**Causa:** Falta incluir Program.hpp

**Solución:**
```cpp
#include "Program.hpp"
```

### Error: "readline/readline.h: No such file or directory"

**Causa:** libreadline no instalada

**Solución:**
```bash
sudo apt-get install libreadline-dev
```

### Warnings sobre "comparison between signed and unsigned"

**Solución:** Usa `.size()` que devuelve `size_t`:
```cpp
for (size_t i = 0; i < vec.size(); ++i) { ... }
// o mejor:
for (const auto& item : vec) { ... }
```

---

## 📊 Checklist de Migración

- [ ] Copiar todos los archivos .hpp a incl/
- [ ] Copiar todos los archivos .cpp a src/
- [ ] Actualizar Makefile
- [ ] Compilar con `make`
- [ ] Probar con config_example.yaml
- [ ] Verificar que funcionen los comandos (start, stop, status)
- [ ] Comprobar logs en /tmp/taskmaster.log
- [ ] Probar Ctrl+C (SIGINT)
- [ ] Probar que los procesos se reinician correctamente
- [ ] Eliminar código C antiguo (opcional)

---

## 🚀 Mejoras Futuras (Opcional)

### 1. Smart Pointers
```cpp
// En lugar de:
CProcessManager* m_process_manager = new CProcessManager();
delete m_process_manager;

// Usa:
std::unique_ptr<CProcessManager> m_process_manager = 
    std::make_unique<CProcessManager>();
// Se elimina automáticamente
```

### 2. Range-based for loops
```cpp
// En lugar de:
for (size_t i = 0; i < programs.size(); ++i) {
    DoSomething(programs[i]);
}

// Usa:
for (const auto& program : programs) {
    DoSomething(program);
}
```

### 3. Lambda functions
```cpp
// Para buscar en vectores:
auto it = std::find_if(programs.begin(), programs.end(),
    [&](const CProgram& p) { 
        return p.GetName() == "nginx"; 
    });
```

---

## 📞 Soporte

Si tienes dudas:
1. Lee **MIGRATION_GUIDE.md** para comparar C vs C++
2. Revisa **README.md** para documentación completa
3. Consulta los comentarios en el código

---

## ✨ ¡Listo!

Tu proyecto está ahora en C++ moderno con:
- ✅ Clases bien diseñadas
- ✅ RAII (gestión automática de recursos)
- ✅ STL (Standard Template Library)
- ✅ Thread-safety
- ✅ Logger profesional
- ✅ Código más limpio y mantenible

**¡A compilar y disfrutar!** 🎉