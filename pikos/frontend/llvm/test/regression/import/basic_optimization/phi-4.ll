; ModuleID = 'phi-4.pp.bc'
source_filename = "phi-4.cpp"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.13.0"

; CHECK-LABEL: Bundle
; CHECK: target-endianness = little-endian
; CHECK: target-pointer-size = 64 bits
; CHECK: target-triple = x86_64-apple-macosx10.13.0

; Function Attrs: noinline norecurse nounwind ssp uwtable
define i32 @main(i32, i8**) #0 !dbg !8 {
  %3 = alloca [10 x i32], align 16
  call void @llvm.dbg.value(metadata i32 %0, metadata !15, metadata !DIExpression()), !dbg !16
  call void @llvm.dbg.value(metadata i8** %1, metadata !17, metadata !DIExpression()), !dbg !18
  call void @llvm.dbg.value(metadata i32 0, metadata !19, metadata !DIExpression()), !dbg !20
  call void @llvm.dbg.declare(metadata [10 x i32]* %3, metadata !21, metadata !DIExpression()), !dbg !25
  %4 = srem i32 %0, 5, !dbg !26
  %5 = icmp eq i32 %4, 0, !dbg !27
  %6 = zext i1 %5 to i8, !dbg !28
  call void @llvm.dbg.value(metadata i8 %6, metadata !29, metadata !DIExpression()), !dbg !28
  br label %7, !dbg !31

; <label>:7:                                      ; preds = %20, %2
  %.0 = phi i32 [ 0, %2 ], [ %21, %20 ], !dbg !32
  call void @llvm.dbg.value(metadata i32 %.0, metadata !19, metadata !DIExpression()), !dbg !20
  %8 = icmp slt i32 %.0, 10, !dbg !33
  br i1 %8, label %9, label %22, !dbg !36

; <label>:9:                                      ; preds = %7
  %10 = trunc i8 %6 to i1, !dbg !37
  br i1 %10, label %11, label %15, !dbg !40

; <label>:11:                                     ; preds = %9
  %12 = xor i32 %.0, 2, !dbg !41
  %13 = sext i32 %.0 to i64, !dbg !42
  %14 = getelementptr inbounds [10 x i32], [10 x i32]* %3, i64 0, i64 %13, !dbg !42
  store i32 %12, i32* %14, align 4, !dbg !43
  br label %19, !dbg !42

; <label>:15:                                     ; preds = %9
  %16 = mul nsw i32 %.0, 2, !dbg !44
  %17 = sext i32 %.0 to i64, !dbg !45
  %18 = getelementptr inbounds [10 x i32], [10 x i32]* %3, i64 0, i64 %17, !dbg !45
  store i32 %16, i32* %18, align 4, !dbg !46
  br label %19

; <label>:19:                                     ; preds = %15, %11
  br label %20, !dbg !47

; <label>:20:                                     ; preds = %19
  %21 = add nsw i32 %.0, 1, !dbg !48
  call void @llvm.dbg.value(metadata i32 %21, metadata !19, metadata !DIExpression()), !dbg !20
  br label %7, !dbg !49, !llvm.loop !50

; <label>:22:                                     ; preds = %7
  ret i32 0, !dbg !52
}
; CHECK: define si32 @main(si32 %1, si8** %2) {
; CHECK: #1 !entry successors={#2, #3} {
; CHECK:   [10 x si32]* $3 = allocate [10 x si32], 1, align 16
; CHECK:   si32 %4 = %1 srem 5
; CHECK: }
; CHECK: #2 predecessors={#1} successors={#4} {
; CHECK:   %4 sieq 0
; CHECK:   ui1 %5 = 1
; CHECK: }
; CHECK: #3 predecessors={#1} successors={#4} {
; CHECK:   %4 sine 0
; CHECK:   ui1 %5 = 0
; CHECK: }
; CHECK: #4 predecessors={#2, #3} successors={#5} {
; CHECK:   ui8 %6 = zext %5
; CHECK:   si32 %.0 = 0
; CHECK: }
; CHECK: #5 predecessors={#4, #10} successors={#6, #7} {
; CHECK: }
; CHECK: #6 predecessors={#5} successors={#8, #9} {
; CHECK:   %.0 silt 10
; CHECK:   ui1 %7 = utrunc %6
; CHECK: }
; CHECK: #7 !exit predecessors={#5} {
; CHECK:   %.0 sige 10
; CHECK:   return 0
; CHECK: }
; CHECK: #8 predecessors={#6} successors={#10} {
; CHECK:   si32 %8 = %.0 sxor 2
; CHECK:   si64 %9 = sext %.0
; CHECK:   si32* %10 = ptrshift $3, 40 * 0, 4 * %9
; CHECK:   store %10, %8, align 4
; CHECK: }
; CHECK: #9 predecessors={#6} successors={#10} {
; CHECK:   si32 %11 = %.0 smul.nw 2
; CHECK:   si64 %12 = sext %.0
; CHECK:   si32* %13 = ptrshift $3, 40 * 0, 4 * %12
; CHECK:   store %13, %11, align 4
; CHECK: }
; CHECK: #10 predecessors={#8, #9} successors={#5} {
; CHECK:   si32 %14 = %.0 sadd.nw 1
; CHECK:   si32 %.0 = %14
; CHECK: }
; CHECK: }

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { noinline norecurse nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus, file: !1, producer: "clang version 8.0.0 (tags/RELEASE_800/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, nameTableKind: GNU)
!1 = !DIFile(filename: "phi-4.cpp", directory: "/Users/marthaud/ikos/ikos-git/frontend/llvm/test/regression/import/basic_optimization")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"clang version 8.0.0 (tags/RELEASE_800/final)"}
!8 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 1, type: !9, scopeLine: 1, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{!11, !11, !12}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !13, size: 64)
!13 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !14, size: 64)
!14 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!15 = !DILocalVariable(name: "argc", arg: 1, scope: !8, file: !1, line: 1, type: !11)
!16 = !DILocation(line: 1, column: 14, scope: !8)
!17 = !DILocalVariable(name: "argv", arg: 2, scope: !8, file: !1, line: 1, type: !12)
!18 = !DILocation(line: 1, column: 27, scope: !8)
!19 = !DILocalVariable(name: "i", scope: !8, file: !1, line: 2, type: !11)
!20 = !DILocation(line: 2, column: 7, scope: !8)
!21 = !DILocalVariable(name: "a", scope: !8, file: !1, line: 2, type: !22)
!22 = !DICompositeType(tag: DW_TAG_array_type, baseType: !11, size: 320, elements: !23)
!23 = !{!24}
!24 = !DISubrange(count: 10)
!25 = !DILocation(line: 2, column: 14, scope: !8)
!26 = !DILocation(line: 3, column: 20, scope: !8)
!27 = !DILocation(line: 3, column: 24, scope: !8)
!28 = !DILocation(line: 3, column: 8, scope: !8)
!29 = !DILocalVariable(name: "flag", scope: !8, file: !1, line: 3, type: !30)
!30 = !DIBasicType(name: "bool", size: 8, encoding: DW_ATE_boolean)
!31 = !DILocation(line: 4, column: 3, scope: !8)
!32 = !DILocation(line: 0, scope: !8)
!33 = !DILocation(line: 4, column: 12, scope: !34)
!34 = distinct !DILexicalBlock(scope: !35, file: !1, line: 4, column: 3)
!35 = distinct !DILexicalBlock(scope: !8, file: !1, line: 4, column: 3)
!36 = !DILocation(line: 4, column: 3, scope: !35)
!37 = !DILocation(line: 5, column: 9, scope: !38)
!38 = distinct !DILexicalBlock(scope: !39, file: !1, line: 5, column: 9)
!39 = distinct !DILexicalBlock(scope: !34, file: !1, line: 4, column: 23)
!40 = !DILocation(line: 5, column: 9, scope: !39)
!41 = !DILocation(line: 6, column: 16, scope: !38)
!42 = !DILocation(line: 6, column: 7, scope: !38)
!43 = !DILocation(line: 6, column: 12, scope: !38)
!44 = !DILocation(line: 8, column: 16, scope: !38)
!45 = !DILocation(line: 8, column: 7, scope: !38)
!46 = !DILocation(line: 8, column: 12, scope: !38)
!47 = !DILocation(line: 9, column: 3, scope: !39)
!48 = !DILocation(line: 4, column: 19, scope: !34)
!49 = !DILocation(line: 4, column: 3, scope: !34)
!50 = distinct !{!50, !36, !51}
!51 = !DILocation(line: 9, column: 3, scope: !35)
!52 = !DILocation(line: 10, column: 3, scope: !8)
