## 一、API整理
## 邱凯佳

1. 读入STL/Obj/3mf/ply/自有格式(Sn3DAlgorithm/include/Sn3DAlgorithm/common/baseTypeIO/baseTypeIO.h)  
    - BaseTriMesh和BaseMesh有什么区别？  
    - read_mesh干啥的？ 凯佳确认用下面的    
    - stl: read_stl_mesh  
        > 最后一个参数是回调函数，算法在一定进度时会调用，并传入当前进度（int 100表示结束）
    - obj: read_obj_with_texture   
        > 也可以读不带纹理的，凯佳确认没问题。因此对调用方透明。最后一个参数传false。  
        另外如果obj没有指定纹理材质文件没事；如果指定了但没找到，会报错。凯佳说一般用的都是指定了的。
    - 3mf: read_3mf_mesh  
        > 最后两个参数暂时填false
    - ply: read_ply_mesh

2. 保存二进制glb(SnUtils/include/gltfIO.h)  
   - int write_gltf_mesh(const Sn3DAlgorithm::BaseTriMesh& mesh, const char* gltffile, bool writeASCII);
        ``` 
        param[in]  mesh    输出的网格，输入参数
        param[in]  gltffile    存储的文件路径
        param[in]  writeASCII  是否写成纯文本形式，传false则保存为二进制形式（glb)
        ```

3. 根据角度生成预览图(SnUtils/include/snUtils)
    - 之前为了go调用方便，这里用的C接口。调用时传入mesh对象的指针即可。
    - 先调用create_render_cpu获取render，传入**宽**、**高**，作为输入参数
    - 再调用create_render_data根据mesh获取renderData
    - 再调用render_with_cpu，输入参数为上两步获取的render、renderData，以及**经度**、**维度**，输出到文件
    - 最后调用清理函数，释放二三步中申请的内存。

4. 综上，上层协议需要关注的参数：  
    （1）生成的预览图的宽、高  
    （2）预览图经、纬度
    （3）读写obj的最后一个参数readVColorIfExists和writeVColorIfExists


## 林忠威

1. 修复(Sn3DAlgorithm/include/Sn3DAlgorithm/algorithmSlicingInclude.h）
    - 第一次检测，get_mesh_error_info里面的needIsct设置为true
    - 调用mesh_auto_fix，needIsct作为输出
    - 第二次检测开始，以上次mesh_auto_fix得到的needIsct作为输入
    - 循环最大迭代次数

2. 综上，上层协议需要关注的一个参数：  
    （1）最大迭代次数

3. 跟他确认如何控制算法占用核心数目

## 贾颜铭

1. 简化(Sn3DAlgorithm/include/Sn3DAlgorithm/algorithm1Include.h)
    - QEM_simplification_omp(TriMesh&, int, double, bool, bool)
    ``` 
	/** \brief 使用QEM方法简化网格 并采用openmp并行加速
	*  \param[in,out]  mesh                 待简化的三角网格
	*  \param[in]      remainNum            简化后剩余顶点的数目
	*  \param[in]      boundaryWeight       保持边界的权重，范围[0,无穷]，权重越大，边界越保持
	*  \param[in]      isCheckFaceFlip      简化过程中，是否判断是否存在翻边
	*  \param[in]      isFeatureSensitive   简化过程中，是否对特征敏感
	*/
    ```
    - 用mesh的成员函数，可以得到顶点数目，来和remainNum对比

2. 平滑(Sn3DAlgorithm/include/Sn3DAlgorithm/algorithm2Include.h)
    - smooth_mesh(TriMesh&, float, int, LapWeightType)
    ```
        /*!
     * @fn	RetVal smooth_mesh(BaseTriMesh& mesh, const float moveDist, const int iter);
     * @brief	网格平滑
     * @date	2017/8/30
     * @param [in,out]	mesh		待处理网格.
     * @param [in]	  	moveDist	顶点移动距离阀值.
     * @param [in]	  	iter		平滑迭代次数.
     * @return	if moveDist &lt;=FLT_EPSILON ||  iter &lt; 1,  RetVal_ILLEGAL_INPUT else RetVal_OK.
     */
    ```
    - set_tbb_num_threads 可以设置tbb全局线程数

3. 上层协议需要关注的两个方面：  
    （1）tbb全局线程数（配置文件）
    （2）简化中，顶点减少百分比
    （3）简化和平滑的其它输入参数

## 张建

1. 读写自有格式(Sn3DAlgorithm/include/Sn3DAlgorithm/algorithm1Include.h)
    - compress_mesh_to_file
    - decompress_file_to_mesh
    跟张建确认过，函数默认参数没必要暴露上去 