## Mesh由渲染器自己管理，避免Triangle每次销毁时，触发一次操作shared_ptr

```c++
loadScene() {
    meshs.push(mesh());
    meshs.push(mesh());
}
```

SAH找不到最好的划分就用穷举法，穷举法找不到就直接构造叶子节点。