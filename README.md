# ConVEX skeleton project

Usage with docker:

```bash
docker build -t "docker-convex" -f Dockerfile .
docker run -v `pwd`:/build/project "docker-convex" sh -c 'cd project && make'
```
