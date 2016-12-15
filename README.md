# Starstruck ConVEX project

This project expects [PROS](http://pros.cs.purdue.edu/) to be installed and available in the `PATH`.

The project may be built by running:

```bash
make
```

And sent to the Cortex by running:

```bash
make flash
```

#### Docker

Usage with docker:

```bash
docker build -t "starstruck-convex" -f Dockerfile .
docker run --rm -v `pwd`:/build "starstruck-convex" make
```

Usage with docker-compose:

```bash
# Start containers
docker-compose up -d
docker attach convex_project_1
# Stop containers
docker-compose stop
```

#### Documentation

* [ChibiOS/RT 2.6.x](http://chibios.sourceforge.net/html/)
* [ConVEX API](https://jpearman.github.io/convex/doxygen/html/)
