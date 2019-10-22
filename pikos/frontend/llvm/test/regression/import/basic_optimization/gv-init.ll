; ModuleID = 'gv-init.pp.bc'
source_filename = "gv-init.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.13.0"

; CHECK-LABEL: Bundle
; CHECK: target-endianness = little-endian
; CHECK: target-pointer-size = 64 bits
; CHECK: target-triple = x86_64-apple-macosx10.13.0

@a = common global [100 x [100 x i32]] zeroinitializer, align 16, !dbg !9
; CHECK: define [100 x [100 x si32]]* @a, align 16, init {
; CHECK: #1 !entry !exit {
; CHECK:   store @a, aggregate_zero, align 1
; CHECK: }
; CHECK: }

@b = global [2 x i32] [i32 1, i32 2], align 4, !dbg !0
; CHECK: define [2 x si32]* @b, align 4, init {
; CHECK: #1 !entry !exit {
; CHECK:   store @b, [1, 2], align 1
; CHECK: }
; CHECK: }

@c = common global i32 0, align 4, !dbg !14
; CHECK: define si32* @c, align 4, init {
; CHECK: #1 !entry !exit {
; CHECK:   store @c, 0, align 1
; CHECK: }
; CHECK: }

@d = global i32 5, align 4, !dbg !6
; CHECK: define si32* @d, align 4, init {
; CHECK: #1 !entry !exit {
; CHECK:   store @d, 5, align 1
; CHECK: }
; CHECK: }

@e = external global i32, align 4
; CHECK: declare si32* @e, align 4

; Function Attrs: noinline nounwind ssp uwtable
define i32 @main(i32, i8**) #0 !dbg !24 {
  call void @llvm.dbg.value(metadata i32 %0, metadata !30, metadata !DIExpression()), !dbg !31
  call void @llvm.dbg.value(metadata i8** %1, metadata !32, metadata !DIExpression()), !dbg !33
  call void @llvm.dbg.value(metadata i32 0, metadata !34, metadata !DIExpression()), !dbg !35
  call void @llvm.dbg.value(metadata i32 0, metadata !36, metadata !DIExpression()), !dbg !37
  br label %3, !dbg !38

; <label>:3:                                      ; preds = %37, %2
  %.01 = phi i32 [ 0, %2 ], [ %38, %37 ], !dbg !39
  %.0 = phi i32 [ 0, %2 ], [ %.1, %37 ], !dbg !37
  call void @llvm.dbg.value(metadata i32 %.0, metadata !36, metadata !DIExpression()), !dbg !37
  call void @llvm.dbg.value(metadata i32 %.01, metadata !34, metadata !DIExpression()), !dbg !35
  %4 = icmp slt i32 %.01, 100, !dbg !40
  br i1 %4, label %5, label %39, !dbg !43

; <label>:5:                                      ; preds = %3
  br label %6, !dbg !44

; <label>:6:                                      ; preds = %34, %5
  %.1 = phi i32 [ %.0, %5 ], [ %35, %34 ], !dbg !39
  call void @llvm.dbg.value(metadata i32 %.1, metadata !36, metadata !DIExpression()), !dbg !37
  %7 = icmp slt i32 %.1, 100, !dbg !45
  br i1 %7, label %8, label %36, !dbg !48

; <label>:8:                                      ; preds = %6
  %9 = srem i32 %.01, 2, !dbg !49
  %10 = icmp eq i32 %9, 0, !dbg !51
  br i1 %10, label %11, label %22, !dbg !52

; <label>:11:                                     ; preds = %8
  %12 = getelementptr inbounds [2 x i32], [2 x i32]* @b, i64 0, i64 0, !dbg !53
  %13 = load i32, i32* %12, align 4, !dbg !53
  %14 = load i32, i32* @c, align 4, !dbg !54
  %15 = add nsw i32 %13, %14, !dbg !55
  %16 = load i32, i32* @e, align 4, !dbg !56
  %17 = sub nsw i32 %15, %16, !dbg !57
  %18 = sext i32 %.01 to i64, !dbg !58
  %19 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @a, i64 0, i64 %18, !dbg !58
  %20 = sext i32 %.1 to i64, !dbg !58
  %21 = getelementptr inbounds [100 x i32], [100 x i32]* %19, i64 0, i64 %20, !dbg !58
  store i32 %17, i32* %21, align 4, !dbg !59
  br label %33, !dbg !58

; <label>:22:                                     ; preds = %8
  %23 = getelementptr inbounds [2 x i32], [2 x i32]* @b, i64 0, i64 1, !dbg !60
  %24 = load i32, i32* %23, align 4, !dbg !60
  %25 = load i32, i32* @d, align 4, !dbg !61
  %26 = add nsw i32 %24, %25, !dbg !62
  %27 = load i32, i32* @e, align 4, !dbg !63
  %28 = sub nsw i32 %26, %27, !dbg !64
  %29 = sext i32 %.01 to i64, !dbg !65
  %30 = getelementptr inbounds [100 x [100 x i32]], [100 x [100 x i32]]* @a, i64 0, i64 %29, !dbg !65
  %31 = sext i32 %.1 to i64, !dbg !65
  %32 = getelementptr inbounds [100 x i32], [100 x i32]* %30, i64 0, i64 %31, !dbg !65
  store i32 %28, i32* %32, align 4, !dbg !66
  br label %33

; <label>:33:                                     ; preds = %22, %11
  br label %34, !dbg !67

; <label>:34:                                     ; preds = %33
  %35 = add nsw i32 %.1, 1, !dbg !68
  call void @llvm.dbg.value(metadata i32 %35, metadata !36, metadata !DIExpression()), !dbg !37
  br label %6, !dbg !69, !llvm.loop !70

; <label>:36:                                     ; preds = %6
  br label %37, !dbg !71

; <label>:37:                                     ; preds = %36
  %38 = add nsw i32 %.01, 1, !dbg !72
  call void @llvm.dbg.value(metadata i32 %38, metadata !34, metadata !DIExpression()), !dbg !35
  br label %3, !dbg !73, !llvm.loop !74

; <label>:39:                                     ; preds = %3
  ret i32 0, !dbg !76
}
; CHECK: define si32 @main(si32 %1, si8** %2) {
; CHECK: #1 !entry successors={#2} {
; CHECK:   si32 %.01 = 0
; CHECK:   si32 %.0 = 0
; CHECK: }
; CHECK: #2 predecessors={#1, #7} successors={#3, #4} {
; CHECK: }
; CHECK: #3 predecessors={#2} successors={#5} {
; CHECK:   %.01 silt 100
; CHECK:   si32 %.1 = %.0
; CHECK: }
; CHECK: #4 !exit predecessors={#2} {
; CHECK:   %.01 sige 100
; CHECK:   return 0
; CHECK: }
; CHECK: #5 predecessors={#3, #10} successors={#6, #7} {
; CHECK: }
; CHECK: #6 predecessors={#5} successors={#8, #9} {
; CHECK:   %.1 silt 100
; CHECK:   si32 %3 = %.01 srem 2
; CHECK: }
; CHECK: #7 predecessors={#5} successors={#2} {
; CHECK:   %.1 sige 100
; CHECK:   si32 %4 = %.01 sadd.nw 1
; CHECK:   si32 %.01 = %4
; CHECK:   si32 %.0 = %.1
; CHECK: }
; CHECK: #8 predecessors={#6} successors={#10} {
; CHECK:   %3 sieq 0
; CHECK:   si32* %5 = ptrshift @b, 8 * 0, 4 * 0
; CHECK:   si32 %6 = load %5, align 4
; CHECK:   si32 %7 = load @c, align 4
; CHECK:   si32 %8 = %6 sadd.nw %7
; CHECK:   si32 %9 = load @e, align 4
; CHECK:   si32 %10 = %8 ssub.nw %9
; CHECK:   si64 %11 = sext %.01
; CHECK:   [100 x si32]* %12 = ptrshift @a, 40000 * 0, 400 * %11
; CHECK:   si64 %13 = sext %.1
; CHECK:   si32* %14 = ptrshift %12, 400 * 0, 4 * %13
; CHECK:   store %14, %10, align 4
; CHECK: }
; CHECK: #9 predecessors={#6} successors={#10} {
; CHECK:   %3 sine 0
; CHECK:   si32* %15 = ptrshift @b, 8 * 0, 4 * 1
; CHECK:   si32 %16 = load %15, align 4
; CHECK:   si32 %17 = load @d, align 4
; CHECK:   si32 %18 = %16 sadd.nw %17
; CHECK:   si32 %19 = load @e, align 4
; CHECK:   si32 %20 = %18 ssub.nw %19
; CHECK:   si64 %21 = sext %.01
; CHECK:   [100 x si32]* %22 = ptrshift @a, 40000 * 0, 400 * %21
; CHECK:   si64 %23 = sext %.1
; CHECK:   si32* %24 = ptrshift %22, 400 * 0, 4 * %23
; CHECK:   store %24, %20, align 4
; CHECK: }
; CHECK: #10 predecessors={#8, #9} successors={#5} {
; CHECK:   si32 %25 = %.1 sadd.nw 1
; CHECK:   si32 %.1 = %25
; CHECK: }
; CHECK: }

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!19, !20, !21, !22}
!llvm.ident = !{!23}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "b", scope: !2, file: !3, line: 2, type: !16, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 8.0.0 (tags/RELEASE_800/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, globals: !5, nameTableKind: GNU)
!3 = !DIFile(filename: "gv-init.c", directory: "/Users/marthaud/ikos/ikos-git/frontend/llvm/test/regression/import/basic_optimization")
!4 = !{}
!5 = !{!0, !6, !9, !14}
!6 = !DIGlobalVariableExpression(var: !7, expr: !DIExpression())
!7 = distinct !DIGlobalVariable(name: "d", scope: !2, file: !3, line: 4, type: !8, isLocal: false, isDefinition: true)
!8 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!9 = !DIGlobalVariableExpression(var: !10, expr: !DIExpression())
!10 = distinct !DIGlobalVariable(name: "a", scope: !2, file: !3, line: 1, type: !11, isLocal: false, isDefinition: true)
!11 = !DICompositeType(tag: DW_TAG_array_type, baseType: !8, size: 320000, elements: !12)
!12 = !{!13, !13}
!13 = !DISubrange(count: 100)
!14 = !DIGlobalVariableExpression(var: !15, expr: !DIExpression())
!15 = distinct !DIGlobalVariable(name: "c", scope: !2, file: !3, line: 3, type: !8, isLocal: false, isDefinition: true)
!16 = !DICompositeType(tag: DW_TAG_array_type, baseType: !8, size: 64, elements: !17)
!17 = !{!18}
!18 = !DISubrange(count: 2)
!19 = !{i32 2, !"Dwarf Version", i32 4}
!20 = !{i32 2, !"Debug Info Version", i32 3}
!21 = !{i32 1, !"wchar_size", i32 4}
!22 = !{i32 7, !"PIC Level", i32 2}
!23 = !{!"clang version 8.0.0 (tags/RELEASE_800/final)"}
!24 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 7, type: !25, scopeLine: 7, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!25 = !DISubroutineType(types: !26)
!26 = !{!8, !8, !27}
!27 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !28, size: 64)
!28 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !29, size: 64)
!29 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!30 = !DILocalVariable(name: "argc", arg: 1, scope: !24, file: !3, line: 7, type: !8)
!31 = !DILocation(line: 7, column: 14, scope: !24)
!32 = !DILocalVariable(name: "argv", arg: 2, scope: !24, file: !3, line: 7, type: !27)
!33 = !DILocation(line: 7, column: 27, scope: !24)
!34 = !DILocalVariable(name: "i", scope: !24, file: !3, line: 8, type: !8)
!35 = !DILocation(line: 8, column: 7, scope: !24)
!36 = !DILocalVariable(name: "j", scope: !24, file: !3, line: 8, type: !8)
!37 = !DILocation(line: 8, column: 14, scope: !24)
!38 = !DILocation(line: 9, column: 3, scope: !24)
!39 = !DILocation(line: 0, scope: !24)
!40 = !DILocation(line: 9, column: 12, scope: !41)
!41 = distinct !DILexicalBlock(scope: !42, file: !3, line: 9, column: 3)
!42 = distinct !DILexicalBlock(scope: !24, file: !3, line: 9, column: 3)
!43 = !DILocation(line: 9, column: 3, scope: !42)
!44 = !DILocation(line: 10, column: 5, scope: !41)
!45 = !DILocation(line: 10, column: 14, scope: !46)
!46 = distinct !DILexicalBlock(scope: !47, file: !3, line: 10, column: 5)
!47 = distinct !DILexicalBlock(scope: !41, file: !3, line: 10, column: 5)
!48 = !DILocation(line: 10, column: 5, scope: !47)
!49 = !DILocation(line: 11, column: 13, scope: !50)
!50 = distinct !DILexicalBlock(scope: !46, file: !3, line: 11, column: 11)
!51 = !DILocation(line: 11, column: 17, scope: !50)
!52 = !DILocation(line: 11, column: 11, scope: !46)
!53 = !DILocation(line: 12, column: 19, scope: !50)
!54 = !DILocation(line: 12, column: 26, scope: !50)
!55 = !DILocation(line: 12, column: 24, scope: !50)
!56 = !DILocation(line: 12, column: 30, scope: !50)
!57 = !DILocation(line: 12, column: 28, scope: !50)
!58 = !DILocation(line: 12, column: 9, scope: !50)
!59 = !DILocation(line: 12, column: 17, scope: !50)
!60 = !DILocation(line: 14, column: 19, scope: !50)
!61 = !DILocation(line: 14, column: 26, scope: !50)
!62 = !DILocation(line: 14, column: 24, scope: !50)
!63 = !DILocation(line: 14, column: 30, scope: !50)
!64 = !DILocation(line: 14, column: 28, scope: !50)
!65 = !DILocation(line: 14, column: 9, scope: !50)
!66 = !DILocation(line: 14, column: 17, scope: !50)
!67 = !DILocation(line: 11, column: 20, scope: !50)
!68 = !DILocation(line: 10, column: 22, scope: !46)
!69 = !DILocation(line: 10, column: 5, scope: !46)
!70 = distinct !{!70, !48, !71}
!71 = !DILocation(line: 14, column: 30, scope: !47)
!72 = !DILocation(line: 9, column: 20, scope: !41)
!73 = !DILocation(line: 9, column: 3, scope: !41)
!74 = distinct !{!74, !43, !75}
!75 = !DILocation(line: 14, column: 30, scope: !42)
!76 = !DILocation(line: 15, column: 3, scope: !24)
