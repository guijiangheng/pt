## Mesh由渲染器自己管理
避免Triangle每次销毁时，触发一次操作shared_ptr。所以Trianlge内部直接保存Mesh的引用而不是shared_ptr。

```c++
loadScene() {
    this.meshs.push(mesh());
    this.meshs.push(mesh());
}
```

SAH找不到最好的划分就用穷举法，穷举法找不到就直接构造叶子节点。

1 弄懂为啥Light要区分DeltaPosition和DeltaDirection。
2 弄懂为啥要sampleLe接口。
3 弄懂VisibilityTest可不可以只要两个顶点，而不是两个Intersection。
4 所有的intersect都不能计算假设ray.d是单位向量化，比如：
    Iteraction::spawnRayTo(const Interaction& isect) const {
        auto d = isect.p - p;
        return Ray(p, d, 1 - ShadowEpsilon)
    }

## Material::computeScatteringFunctions时可以不为isect生成bsdf,比如Matte材质Kd=0时
