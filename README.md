# TaskMaster - C++ Migration

## 📋 Descripción

TaskMaster es un supervisor de procesos similar a `supervisord`, migrado de C a C++ moderno.

## 🏗️ Arquitectura del Proyecto

### Clases Principales

#### 1. **CConfigParser**
- Parsea archivos YAML de configuración
- Carga los programas y sus configuraciones
- Maneja variables de entorno

#### 2. **CProgram**
- Representa un programa a supervisar
- Contiene configuración: comando, numprocs, autorestart, etc.
- Maneja variables de entorno específicas del programa

#### 3. **CProcessManager**
- Gestiona el ciclo de vida de los procesos
- Maneja arranque, parada y reinicio
- Implementa políticas de autorestart
- Monitorea estados de procesos

#### 4. **CSignalHandler**
- Maneja señales del sistema (SIGINT, SIGCHLD, SIGHUP)
- Uso de `std::atomic` para thread-safety
- Permite comunicación asíncrona con el main loop

#### 5. **CTaskmasterShell**
- Shell interactiva con readline
- Comandos: start, stop, restart, status, reload, exit
- Procesamiento de comandos en tiempo real

#### 6. **CLogger**
- Sistema de logging thread-safe
- Niveles: DEBUG, INFO, WARNING, ERROR, FATAL
- Salida a consola y/o archivo

#### 7. **Utils** (namespace)
- Funciones auxiliares
- Conversión de strings, validaciones
- Utilidades de filesystem

## 📁 Estructura de Archivos

```
taskmaster/
├── incl/
│   ├── ConfigParser.hpp
│   ├── Program.hpp
│   ├── ProcessManager.hpp
│   ├── SignalHandler.hpp
│   ├── TaskmasterShell.hpp
│   ├── Logger.hpp
│   ├── Utils.hpp
│   └── parse_utils.hpp
├── src/
│   ├── main_new.cpp
│   ├── ConfigParser.cpp
│   ├── Program.cpp
│   ├── ProcessManager.cpp
│   ├── SignalHandler.cpp
│   ├── TaskmasterShell.cpp
│   ├── Logger.cpp
│   └── Utils.cpp
├── aux/
│   └── parse_utils.cpp
├── bin/                 (generado)
│   └── taskmaster
├── obj/                 (generado)
└── Makefile
```

## 🔧 Compilación

### Requisitos
- g++ con soporte C++17
- libreadline-dev
- pthread

### Instalar dependencias (Ubuntu/Debian)
```bash
sudo apt-get install g++ libreadline-dev
```

### Compilar
```bash
make        # Compilar el proyecto
make clean  # Limpiar objetos
make fclean # Limpiar todo
make re     # Recompilar
```

## 🚀 Uso

### Ejecutar TaskMaster
```bash
./bin/taskmaster config.yaml
```

### Comandos Disponibles

En el prompt `TaskMaster> `:

- **start <program>** - Iniciar un programa
- **stop <program>** - Detener un programa
- **restart <program>** - Reiniciar un programa
- **status [program]** - Ver estado de procesos
- **reload** - Recargar configuración (pendiente)
- **help** - Mostrar ayuda
- **exit/quit** - Salir de TaskMaster

### Ejemplo de Sesión

```
$ ./bin/taskmaster config.yaml
[2025-01-15 10:30:00] [INFO] === TaskMaster Starting ===
[2025-01-15 10:30:00] [INFO] Loaded 2 programs from configuration
[2025-01-15 10:30:00] [INFO] Starting autostart programs...
[2025-01-15 10:30:00] [INFO] Process 'nginx' [0] started with PID: 1234

TaskMaster> status
=== Process Status ===
nginx[0]: RUNNING (PID: 1234)
app[0]: STOPPED

TaskMaster> start app
✅ Starting program: app

TaskMaster> exit
Shutting down TaskMaster...
✅ TaskMaster shut down cleanly
```

## 📝 Configuración YAML

Ejemplo de archivo de configuración:

```yaml
programs:
  nginx:
    cmd: "/usr/sbin/nginx"
    numprocs: 1
    autostart: true
    autorestart: unexpected
    exitcodes: [0, 2]
    startretries: 3
    starttime: 5
    stopsignal: "TERM"
    stoptime: 10
    stdout: "/var/log/nginx_stdout.log"
    stderr: "/var/log/nginx_stderr.log"
    workingdir: "/var/www"
    umask: "022"
    env:
      PATH: "/usr/local/bin:/usr/bin"
      USER: "www-data"

  myapp:
    cmd: "./bin/myapp"
    numprocs: 2
    autostart: false
    autorestart: always
    exitcodes: [0]
    startretries: 5
    stdout: "/tmp/myapp.log"
```

## 🔄 Diferencias C vs C++

### Mejoras Implementadas

1. **Gestión de Memoria**
   - ❌ C: `malloc/free` manual
   - ✅ C++: RAII, smart pointers (próximamente)

2. **Strings**
   - ❌ C: `char*`, funciones manuales
   - ✅ C++: `std::string`

3. **Contenedores**
   - ❌ C: Arrays estáticos, listas enlazadas manuales
   - ✅ C++: `std::vector`, `std::map`

4. **Encapsulación**
   - ❌ C: Structs con funciones separadas
   - ✅ C++: Clases con métodos

5. **Thread Safety**
   - ❌ C: `pthread_mutex_t`
   - ✅ C++: `std::mutex`, `std::atomic`

6. **Logging**
   - ❌ C: `ft_printf` disperso
   - ✅ C++: Clase `CLogger` centralizada

## 🎯 Estado del Proyecto

### ✅ Completado
- Parser YAML
- Estructuras de datos (Program, Config)
- ProcessManager básico
- SignalHandler
- TaskmasterShell con readline
- Logger thread-safe
- Utils

### 🚧 En Progreso
- Timeouts de starttime/stoptime
- SIGHUP reload de configuración
- Redirección robusta de I/O

### 📋 Pendiente
- Múltiples instancias (numprocs > 1) - testing
- Tests unitarios
- Manejo de edge cases

## 🐛 Debug

Logs se guardan en: `/tmp/taskmaster.log`

Para más verbosidad, cambiar nivel en `main_new.cpp`:
```cpp
CLogger::Init("/tmp/taskmaster.log", LogLevel::DEBUG);
```

## 📚 Referencias

- [Supervisor Documentation](http://supervisord.org/)
- [C++ Best Practices](https://github.com/cpp-best-practices)
- [Modern C++ Design Patterns](https://refactoring.guru/design-patterns)

## 👥 Autores

- **davgalle** - Versión C original
- **nicgonza2** - Migración a C++

## 📄 Licencia

Proyecto educativo - 42 School