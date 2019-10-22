; ModuleID = 'vla.pp.bc'
source_filename = "vla.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.13.0"

; CHECK-LABEL: Bundle
; CHECK: target-endianness = little-endian
; CHECK: target-pointer-size = 64 bits
; CHECK: target-triple = x86_64-apple-macosx10.13.0

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1
; CHECK: define [3 x si8]* @.str, align 1, init {
; CHECK: #1 !entry !exit {
; CHECK:   store @.str, [37, 100, 0], align 1
; CHECK: }
; CHECK: }

declare i32 @scanf(i8*, ...) #3
; CHECK: declare si32 @ar.libc.scanf(si8*, ...)

; Function Attrs: nounwind
declare void @llvm.stackrestore(i8*) #2
; CHECK: declare void @ar.stackrestore(si8*)

; Function Attrs: nounwind
declare i8* @llvm.stacksave() #2
; CHECK: declare si8* @ar.stacksave()

; Function Attrs: noinline nounwind ssp uwtable
define void @foo(i32) #0 !dbg !8 {
  call void @llvm.dbg.value(metadata i32 %0, metadata !12, metadata !DIExpression()), !dbg !13
  %2 = zext i32 %0 to i64, !dbg !14
  %3 = call i8* @llvm.stacksave(), !dbg !14
  %4 = alloca i32, i64 %2, align 16, !dbg !14
  call void @llvm.dbg.value(metadata i64 %2, metadata !15, metadata !DIExpression()), !dbg !17
  call void @llvm.dbg.declare(metadata i32* %4, metadata !18, metadata !DIExpression()), !dbg !22
  call void @llvm.dbg.value(metadata i32 0, metadata !23, metadata !DIExpression()), !dbg !24
  br label %5, !dbg !25

; <label>:5:                                      ; preds = %11, %1
  %.0 = phi i32 [ 0, %1 ], [ %12, %11 ], !dbg !27
  call void @llvm.dbg.value(metadata i32 %.0, metadata !23, metadata !DIExpression()), !dbg !24
  %6 = icmp slt i32 %.0, %0, !dbg !28
  br i1 %6, label %7, label %13, !dbg !30

; <label>:7:                                      ; preds = %5
  %8 = mul nsw i32 %.0, %.0, !dbg !31
  %9 = sext i32 %.0 to i64, !dbg !33
  %10 = getelementptr inbounds i32, i32* %4, i64 %9, !dbg !33
  store i32 %8, i32* %10, align 4, !dbg !34
  br label %11, !dbg !35

; <label>:11:                                     ; preds = %7
  %12 = add nsw i32 %.0, 1, !dbg !36
  call void @llvm.dbg.value(metadata i32 %12, metadata !23, metadata !DIExpression()), !dbg !24
  br label %5, !dbg !37, !llvm.loop !38

; <label>:13:                                     ; preds = %5
  %14 = mul nsw i32 %0, %0, !dbg !40
  %15 = sext i32 %0 to i64, !dbg !41
  %16 = getelementptr inbounds i32, i32* %4, i64 %15, !dbg !41
  store i32 %14, i32* %16, align 4, !dbg !42
  call void @llvm.stackrestore(i8* %3), !dbg !43
  ret void, !dbg !43
}
; CHECK: define void @foo(si32 %1) {
; CHECK: #1 !entry successors={#2} {
; CHECK:   ui32 %2 = bitcast %1
; CHECK:   ui64 %3 = zext %2
; CHECK:   si8* %4 = call @ar.stacksave()
; CHECK:   si32* $5 = allocate si32, %3, align 16
; CHECK:   si32 %.0 = 0
; CHECK: }
; CHECK: #2 predecessors={#1, #3} successors={#3, #4} {
; CHECK: }
; CHECK: #3 predecessors={#2} successors={#2} {
; CHECK:   %.0 silt %1
; CHECK:   si32 %6 = %.0 smul.nw %.0
; CHECK:   si64 %7 = sext %.0
; CHECK:   si32* %8 = ptrshift $5, 4 * %7
; CHECK:   store %8, %6, align 4
; CHECK:   si32 %9 = %.0 sadd.nw 1
; CHECK:   si32 %.0 = %9
; CHECK: }
; CHECK: #4 !exit predecessors={#2} {
; CHECK:   %.0 sige %1
; CHECK:   si32 %10 = %1 smul.nw %1
; CHECK:   si64 %11 = sext %1
; CHECK:   si32* %12 = ptrshift $5, 4 * %11
; CHECK:   store %12, %10, align 4
; CHECK:   call @ar.stackrestore(%4)
; CHECK:   return
; CHECK: }
; CHECK: }

; Function Attrs: noinline nounwind ssp uwtable
define i32 @main(i32, i8**) #0 !dbg !44 {
  %3 = alloca i32, align 4
  call void @llvm.dbg.value(metadata i32 %0, metadata !50, metadata !DIExpression()), !dbg !51
  call void @llvm.dbg.value(metadata i8** %1, metadata !52, metadata !DIExpression()), !dbg !53
  call void @llvm.dbg.declare(metadata i32* %3, metadata !54, metadata !DIExpression()), !dbg !55
  %4 = getelementptr inbounds [3 x i8], [3 x i8]* @.str, i32 0, i32 0, !dbg !56
  %5 = call i32 (i8*, ...) @scanf(i8* %4, i32* %3), !dbg !56
  %6 = load i32, i32* %3, align 4, !dbg !57
  call void @foo(i32 %6), !dbg !58
  ret i32 0, !dbg !59
}
; CHECK: define si32 @main(si32 %1, si8** %2) {
; CHECK: #1 !entry !exit {
; CHECK:   si32* $3 = allocate si32, 1, align 4
; CHECK:   si8* %4 = ptrshift @.str, 3 * 0, 1 * 0
; CHECK:   si32 %5 = call @ar.libc.scanf(%4, $3)
; CHECK:   si32 %6 = load $3, align 4
; CHECK:   call @foo(%6)
; CHECK:   return 0
; CHECK: }
; CHECK: }

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { nounwind }
attributes #3 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 8.0.0 (tags/RELEASE_800/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, nameTableKind: GNU)
!1 = !DIFile(filename: "vla.c", directory: "/Users/marthaud/ikos/ikos-git/frontend/llvm/test/regression/import/basic_optimization")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"clang version 8.0.0 (tags/RELEASE_800/final)"}
!8 = distinct !DISubprogram(name: "foo", scope: !1, file: !1, line: 2, type: !9, scopeLine: 2, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{null, !11}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !DILocalVariable(name: "n", arg: 1, scope: !8, file: !1, line: 2, type: !11)
!13 = !DILocation(line: 2, column: 14, scope: !8)
!14 = !DILocation(line: 3, column: 3, scope: !8)
!15 = !DILocalVariable(name: "__vla_expr0", scope: !8, type: !16, flags: DIFlagArtificial)
!16 = !DIBasicType(name: "long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!17 = !DILocation(line: 0, scope: !8)
!18 = !DILocalVariable(name: "a", scope: !8, file: !1, line: 3, type: !19)
!19 = !DICompositeType(tag: DW_TAG_array_type, baseType: !11, elements: !20)
!20 = !{!21}
!21 = !DISubrange(count: !15)
!22 = !DILocation(line: 3, column: 7, scope: !8)
!23 = !DILocalVariable(name: "i", scope: !8, file: !1, line: 3, type: !11)
!24 = !DILocation(line: 3, column: 13, scope: !8)
!25 = !DILocation(line: 4, column: 8, scope: !26)
!26 = distinct !DILexicalBlock(scope: !8, file: !1, line: 4, column: 3)
!27 = !DILocation(line: 0, scope: !26)
!28 = !DILocation(line: 4, column: 17, scope: !29)
!29 = distinct !DILexicalBlock(scope: !26, file: !1, line: 4, column: 3)
!30 = !DILocation(line: 4, column: 3, scope: !26)
!31 = !DILocation(line: 5, column: 14, scope: !32)
!32 = distinct !DILexicalBlock(scope: !29, file: !1, line: 4, column: 27)
!33 = !DILocation(line: 5, column: 5, scope: !32)
!34 = !DILocation(line: 5, column: 10, scope: !32)
!35 = !DILocation(line: 6, column: 3, scope: !32)
!36 = !DILocation(line: 4, column: 23, scope: !29)
!37 = !DILocation(line: 4, column: 3, scope: !29)
!38 = distinct !{!38, !30, !39}
!39 = !DILocation(line: 6, column: 3, scope: !26)
!40 = !DILocation(line: 7, column: 12, scope: !8)
!41 = !DILocation(line: 7, column: 3, scope: !8)
!42 = !DILocation(line: 7, column: 8, scope: !8)
!43 = !DILocation(line: 8, column: 1, scope: !8)
!44 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 10, type: !45, scopeLine: 10, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!45 = !DISubroutineType(types: !46)
!46 = !{!11, !11, !47}
!47 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !48, size: 64)
!48 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !49, size: 64)
!49 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!50 = !DILocalVariable(name: "argc", arg: 1, scope: !44, file: !1, line: 10, type: !11)
!51 = !DILocation(line: 10, column: 14, scope: !44)
!52 = !DILocalVariable(name: "argv", arg: 2, scope: !44, file: !1, line: 10, type: !47)
!53 = !DILocation(line: 10, column: 27, scope: !44)
!54 = !DILocalVariable(name: "v", scope: !44, file: !1, line: 11, type: !11)
!55 = !DILocation(line: 11, column: 7, scope: !44)
!56 = !DILocation(line: 12, column: 3, scope: !44)
!57 = !DILocation(line: 13, column: 7, scope: !44)
!58 = !DILocation(line: 13, column: 3, scope: !44)
!59 = !DILocation(line: 14, column: 3, scope: !44)
