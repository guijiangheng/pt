## Mesh由渲染器自己管理，避免Triangle每次销毁时，触发一次操作shared_ptr

```c++
loadScene() {
    meshs.push(mesh());
    meshs.push(mesh());
}
```