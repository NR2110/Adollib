#pragma once
#include <vector>
#include <list>
#include <assert.h>
#include "../Math/quaternion.h"
#include "../Math/matrix.h"
#include "ALP_collider.h"

namespace Adollib {
	namespace Physics_function {
		//Contactクラス用のnamespace
		namespace Contacts {
			static const int contact_max_per_pair = 4; //一つのpairで衝突の解散を行う最大の数

			//拘束
			struct Constraint {
				Vector3 axis;			//拘束軸
				float jacDiagInv = 0;	//拘束式の分母
				float rhs = 0;			//初期拘束力
				float tangent_rhs = 0;  //拘束軸が法線の面上での拘束力
				float lowerlimit = 0;	//拘束力の下限
				float upperlimit = 0;	//拘束力の上限
				float accuminpulse = 0; //累積される拘束力
			};


			//衝突点情報
			struct Contactpoint {
				float distance = 0;		//衝突深度
				Vector3 point[2];		//衝突点
				Vector3 normal;			//衝突点の法線ベクトル
				Constraint constraint[3];  //拘束

				//初期化
				void reset() {
					constraint[0].accuminpulse = 0.0f;
					constraint[1].accuminpulse = 0.0f;
					constraint[2].accuminpulse = 0.0f;
				}
			};

			//衝突処理用
			class Contact {
			public:
				int contact_num = 0;    //衝突点の数
				float friction = 0;		//その衝突の摩擦力
				Contactpoint contactpoints[contact_max_per_pair]; //衝突点情報

				Contact() {
				}

				//::::::::
				// 衝突点の追加
				// float penetration : 貫通量
				// vector3 normal : point[0]からpoint[1]衝突法線 (ワールド座標系)
				// vector3 contactpointA : 衝突点 (point[0]のローカル座標系)
				// vector3 contactpointB : 衝突点 (point[1]のローカル座標系)
				//::::::::
				void addcontact(
					float penetration,
					const Vector3& normal,
					const Vector3& contact_pointA,
					const Vector3& contact_pointB
				);

				//:::::::
				// 衝突点の衝突が今も存在するかの確認
				// vector3 pointA : Rigitbody[0]の位置
				// vector3 rotA : Rigitbody[0]の姿勢
				// vector3 pointB : Rigitbody[1]の位置
				// vector3 rotB : Rigitbody[1]の姿勢
				// 存在しない衝突点があればtrueを返す(sleep処理のために)
				//:::::::
				bool chack_remove_contact_point(
					const Collider_shape* shape0,
					const Collider_shape* shape1
				);

				//初期化
				void reset() {
					contact_num = 0;
					for (int i = 0; i < contact_max_per_pair; i++) {
						contactpoints[i].reset();
					}
				};

			private:
				//:::::::::
				// addcontactで使用
				// 除去するContactPointを見つける
				// vector3 newPoint : 追加するCPの新しい衝突点
				// float newDistance : 追加するCPの新しい貫通量
				//:::::::::
				int find_exclusion_point(const Vector3& newPoint, float newDistance);

				//::::::::::
				// find_exclusion_pointで使用
				// 4点で作られる4角形の面積^2を返す
				//::::::::::
				float area_4points(const Vector3& p1, const Vector3& p2, const Vector3& p3, const Vector3& p4);

				//::::::::
				// i 番目の衝突点を破棄 その後詰める
				//::::::::
				void remove_contactpoint(int i);

				//::::::::
				// すでにその衝突点が作られていた場合contactpointの番号を返す
				// 作られていなければ-1を返す
				// vector3 contactpointA : 衝突点 (point[0]のローカル座標系)
				// vector3 contactpointB : 衝突点 (point[1]のローカル座標系)
				// vector3 normal : 衝突法線 (ワールド座標系)
				//::::::::
				int find_contact_point(
					const Vector3& contact_pointA,
					const Vector3& contact_pointB,
					const Vector3& normal
				);
			};

			enum class Pairtype {
				new_pair, // 新規
				keep_pair, // 維持
			};

			struct Contact_pair {

				Pairtype type = Pairtype::new_pair; //衝突の種類(前フレームからある衝突かどうか)

				union {
					struct {
						Collider_shape* body[2]; //接触したobject
					};
					double key; //ポインタ2つからなるユニークな数字
				};

				Contact contacts; //衝突の情報

				bool check_oncoll_only = false;
			};

			struct Collider_2 {
				Collider_shape* body;
				std::list<Collider_shape*> bodylists;
			};

			// ソート
			// param[in,out] d ソートするデータの配列
			// param buff ソート用のバッファ（入力データと同サイズ）
			// param n データの数

			static int partition(std::vector<Contacts::Contact_pair>& array, int l, int r) {
				auto& pivot = array[r];
				int i = (l - 1);

				for (int j = l; j <= r - 1; j++) {
					if (array[j].key <= pivot.key) {
						i++;

						const auto t = array[i];
						array[i] = array[j];
						array[j] = t;
					}
				}

				{
					const auto t = array[i + 1];
					array[i + 1] = array[r];
					array[r] = t;
				}
				return (i + 1);
			}

			static void Contact_pair_quick_sort(std::vector<Contacts::Contact_pair>& array, int l, int r) {
				if (l < r) {
					int pivot = partition(array, l, r);
					Contact_pair_quick_sort(array, l, pivot - 1);
					Contact_pair_quick_sort(array, pivot + 1, r);
				}
			}

			//static void Contact_pair_quick_sort(Contact_pair* d, Contact_pair* buff, int n)
			//{
			//	int n1 = n >> 1;
			//	int n2 = n - n1;
			//	if (n1 > 1) Contact_pair_quick_sort(d, buff, n1);
			//	if (n2 > 1) Contact_pair_quick_sort(d + n1, buff, n2);

			//	{
			//		Contact_pair* d1 = d;
			//		Contact_pair* d2 = d + n1;
			//		int i = 0, j = 0;

			//		while (i < n1 && j < n2) {
			//			if (d1[i].key < d2[j].key) {
			//				buff[i + j] = d1[i++];
			//			}
			//			else {
			//				buff[i + j] = d2[j++];
			//			}
			//		}

			//		if (i < n1) {
			//			while (i < n1) {
			//				buff[i + j] = d1[i++];
			//			}
			//		}
			//		else if (j < n2) {
			//			while (j < n2) {
			//				buff[i + j] = d2[j++];
			//			}
			//		}

			//		for (int k = 0; k < (n1 + n2); k++) {
			//			d1[k] = buff[k];
			//		}
			//	}

			//};




		}
	}
}