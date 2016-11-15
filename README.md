# ConVEX skeleton project

Usage with docker:

```bash
docker build -t "docker-convex" -f Dockerfile .
docker run -v `pwd`:/build/project "docker-convex" sh -c 'cd project && make'
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
