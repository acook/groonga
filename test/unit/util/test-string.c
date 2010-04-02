/* -*- c-basic-offset: 2; coding: utf-8 -*- */
/*
  Copyright (C) 2008-2010  Kouhei Sutou <kou@clear-code.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License version 2.1 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <groonga.h>

#include <groonga_in.h>
#include <stdlib.h>
#include <str.h>

#include <gcutter.h>
#include <glib/gstdio.h>

#include "../lib/grn-assertions.h"

void data_normalize_utf8(void);
void test_normalize_utf8(gpointer data);
void test_charlen_nonnull_broken_utf8(void);

static grn_ctx context;

static const gchar text_ja_utf8[] =
  "Groongaは組み込み型の全文検索エンジンです。DBMSやスクリプト言語処理系等に\n"
  "組み込むことによって、その全文検索機能を強化することができます。n-gram\n"
  "インデックスと単語インデックスの特徴を兼ね備えた、高速かつ高精度な転置\n"
  "インデックスタイプのエンジンです。コンパクトな実装ですが、大規模な文書\n"
  "量と検索要求を処理できるように設計されています。また、純粋なn-gramイン\n"
  "デックスの作成も可能です。";

static const gchar normalized_text_ja_utf8[] =
  "groongaは組み込み型の全文検索エンジンです。dbmsやスクリプト言語処理系等に"
  "組み込むことによって、その全文検索機能を強化することができます。n-gram"
  "インデックスと単語インデックスの特徴を兼ね備えた、高速かつ高精度な転置"
  "インデックスタイプのエンジンです。コンパクトな実装ですが、大規模な文書"
  "量と検索要求を処理できるように設計されています。また、純粋なn-gramイン"
  "デックスの作成も可能です。";

void
setup (void)
{
  grn_ctx_init(&context, GRN_CTX_USE_QL);
}

void
teardown (void)
{
  grn_ctx_fin(&context);
}

void
data_normalize_utf8(void)
{
#define ADD_DATUM(label, expected, input)               \
  gcut_add_datum(label,                                 \
                 "expected", G_TYPE_STRING, expected,   \
                 "input", G_TYPE_STRING, input,         \
                 NULL)

  ADD_DATUM("with newlines",
            "groongaは組み込み型の全文検索エンジンです。"
            "dbmsやスクリプト言語処理系等に組み込むこと"
            "によって、その全文検索機能を強化することが"
            "できます。n-gramインデックスと単語インデッ"
            "クスの特徴を兼ね備えた、高速かつ高精度な転"
            "置インデックスタイプのエンジンです。コンパ"
            "クトな実装ですが、大規模な文書量と検索要求"
            "を処理できるように設計されています。また、"
            "純粋なn-gramインデックスの作成も可能です。",

            "groongaは組み込み型の全文検索エンジンです。\n"
            "DBMSやスクリプト言語処理系等に組み込むこと\n"
            "によって、その全文検索機能を強化することが\n"
            "できます。n-gramインデックスと単語インデッ\n"
            "クスの特徴を兼ね備えた、高速かつ高精度な転\n"
            "置インデックスタイプのエンジンです。コンパ\n"
            "クトな実装ですが、大規模な文書量と検索要求\n"
            "を処理できるように設計されています。また、\n"
            "純粋なn-gramインデックスの作成も可能です。");

  ADD_DATUM("large normalization",
            "キロメートルキロメートルキロメートルキロメートル",
            "㌖㌖㌖㌖");

  ADD_DATUM("tilde and fullwidth tilde and wave dash",
            "~～〜",
            "~~~");

#undef ADD_DATUM
}

void
test_normalize_utf8(gpointer data)
{
  const gchar *expected, *input;
  grn_str *string;
  const gchar *normalized_text;
  guint normalized_text_len;
  int flags;

  GRN_CTX_SET_ENCODING(&context, GRN_ENC_UTF8);
  flags = GRN_STR_NORMALIZE | GRN_STR_WITH_CHECKS | GRN_STR_WITH_CTYPES;
  input = gcut_data_get_string(data, "input");
  string = grn_str_open(&context, input, strlen(input), flags);
  normalized_text = cut_take_strndup(string->norm, string->norm_blen);
  normalized_text_len = string->norm_blen;
  grn_test_assert(grn_str_close(&context, string));

  expected = gcut_data_get_string(data, "expected");
  cut_assert_equal_string(expected, normalized_text);
  cut_assert_equal_int(strlen(expected), normalized_text_len);
}

void
test_charlen_nonnull_broken_utf8(void)
{
  const gchar utf8[] = "あ";
  GRN_CTX_SET_ENCODING(&context, GRN_ENC_UTF8);
  cut_assert_equal_uint(0, grn_charlen(&context, utf8, utf8 + 1));
}
